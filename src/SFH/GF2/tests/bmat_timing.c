// Marc B. Reynolds, 2011-2024
// Public Domain under http://unlicense.org, see link for details.

// for pthread_setaffinity_np which I'm not using ATM.
//#define _GNU_SOURCE
//#define __USE_GNU

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
//#include <errno.h> // sigh
#include <pthread.h>
#include <sched.h>

#include "../bmat_everything.h"

#include "bmat_test.h"


#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))

// number of 64x64 matrices in arrays below
#define SINK_CHUNKS 1024

bmat_def_64(sink_data,   SINK_CHUNKS);
bmat_def_64(fake_data_0, SINK_CHUNKS);
bmat_def_64(fake_data_1, SINK_CHUNKS);
bmat_def_64(fake_data_2, SINK_CHUNKS);

uint64_t* m0;
uint64_t* m1;
uint64_t* m2;

#define STRINGIFY(S)  STRINGIFY_(S)
#define STRINGIFY_(S) #S

//--------------------------------------------------

// f(m) calls


// f(m,m) calls
#define FOR_2M_FUNCS(DO)     \
    DO(bmat_kernel_8)        \
    DO(bmat_kernel_16)       \
    DO(bmat_kernel_32)       \
    DO(bmat_kernel_64)       \
    DO(bmat_inverse_8)       \
    DO(bmat_inverse_16)      \
    DO(bmat_inverse_32)      \
    DO(bmat_inverse_64)      \
    DO(bmat_transpose_8)     \
    DO(bmat_transpose_16)    \
    DO(bmat_transpose_32)    \
    DO(bmat_transpose_64)

#define FOR_2R_FUNCS(DO)      \
    DO(bmat_transpose_8_ref)  \
    DO(bmat_transpose_16_ref) \
    DO(bmat_transpose_32_ref) \
    DO(bmat_transpose_64_ref)

// f(m,m,m) calls
#if 1
#define FOR_3M_FUNCS(DO) \
    DO(bmat_mul_8)       \
    DO(bmat_mul_16)      \
    DO(bmat_mul_32)      \
    DO(bmat_mul_64)      \
    DO(bmat_mult_8)      \
    DO(bmat_mult_16)     \
    DO(bmat_mult_32)     \
    DO(bmat_mult_64)
#else
#define FOR_3M_FUNCS(DO) \
    DO(bmat_mul_8)       \
    DO(m4ri_wrap_mm_8)   \
    DO(bmat_mul_16)      \
    DO(m4ri_wrap_mm_16)  \
    DO(bmat_mul_32)      \
    DO(m4ri_wrap_mm_32)  \
    DO(bmat_mul_64)      \
    DO(m4ri_wrap_mm_64)  \
    DO(bmat_mult_8)      \
    DO(bmat_mult_16)     \
    DO(bmat_mult_32)     \
    DO(bmat_mult_64)
#endif

#define FOR_3R_FUNCS(DO) \
    DO(bmat_mul_8_ref)   \
    DO(bmat_mul_16_ref)  \
    DO(bmat_mul_32_ref)  \
    DO(bmat_mul_64_ref)  \
    DO(bmat_mult_8_ref)  \
    DO(bmat_mult_16_ref) \
    DO(bmat_mult_32_ref) \
    DO(bmat_mult_64_ref)

#define FOR_1MI_FUNCS(DO) \
    DO(bmat_rank_8)       \
    DO(bmat_rank_16)      \
    DO(bmat_rank_32)      \
    DO(bmat_rank_64)      \
    DO(bmat_charpoly_8)   \
    DO(bmat_charpoly_16)  \
    DO(bmat_charpoly_32)  \
    DO(bmat_charpoly_64)

//--------------------------------------------------
volatile uint64_t sink64 = 0;

#define WRAP_1M(N)  BMAT_FLATTEN void w_##N(void) { N(m0); }
#define WRAP_2M(N)  BMAT_FLATTEN void w_##N(void) { N(m0,(void*)m1); }
#define WRAP_3M(N)  BMAT_FLATTEN void w_##N(void) { N(m0,m1,m2); }

#define WRAP_1MI(N) BMAT_FLATTEN void w_##N(void) { sink64 += (uint64_t)N(m0); }

// expand wrappers
//FOR_1M_FUNCS(WRAP_1M)
FOR_2M_FUNCS(WRAP_2M)
FOR_2R_FUNCS(WRAP_2M)
FOR_3M_FUNCS(WRAP_3M)
FOR_3R_FUNCS(WRAP_3M)
FOR_1MI_FUNCS(WRAP_1MI)

// add flags and such for better ergo
// (selective running, etc)
typedef struct {
  void (*f)(void);
  char*   name;
} func_entry_t;

     
#define ENTRY(X) { .f=&w_##X, .name=STRINGIFY(X) },
     
func_entry_t func_table[] =
{
//FOR_1M_FUNCS(ENTRY)
  FOR_1MI_FUNCS(ENTRY)
  FOR_3M_FUNCS(ENTRY)
//FOR_2R_FUNCS(ENTRY)
  FOR_2M_FUNCS(ENTRY)
 //FOR_3R_FUNCS(ENTRY)
};

//--------------------------------------------------------------------------

static inline double f64_sqrt(double a)
{
  return _mm_cvtsd_f64(_mm_sqrt_sd(_mm_undefined_pd(),_mm_set_sd(a)));
}

#if 1
const uint32_t time_count  = 1<<15;   // number of func calls/trial
const  int32_t time_trials =    32;   // number of trials
#else
const uint32_t time_count  =    64;   // number of func calls/trial
const  int32_t time_trials = 1<<10;   // number of trials
#endif
const  int32_t time_retry  =    15;   // max number of retries
const  int32_t time_rerun  =     0;   // max number of retries
const   double time_rlimit =  0.10;   // mean/std threshold (meh)

// 
char*  time_string_cycles = "mean ± std (cycles) ";
char*  time_string_ns     = "  mean ± std (ns)   ";
bool   time_cycles = false;
char*  time_string;

const double time_scale = 1.0/((double)time_count);

static inline uint64_t time_get(void)
{
  if (time_cycles)
    return _rdtsc();

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (1000*1000*1000*(uint64_t)ts.tv_sec + (uint64_t)ts.tv_nsec);
}

static inline double doubletime(void)
{
  return (double)time_get();
}

// Welford's method for streaming mean/variance/stdev
// --- just being lazy. no "streaming" is involved
// here.
typedef struct { double n,m,s; } seq_stats_t;

static inline void seq_stats_init(seq_stats_t* d)
{
  memset(d,0,sizeof(seq_stats_t));
}

void seq_stats_add(seq_stats_t* d, double x)
{
  d->n += 1.0;

  double m  = d->m;
  double s  = d->s;
  double dm = x-m;
  
  d->m = m + dm/d->n;
  d->s = s + dm*(x-d->m);
}

static inline double seq_stats_mean(seq_stats_t* d)     { return d->m; }
static inline double seq_stats_variance(seq_stats_t* d) { return d->s/(d->n-1.0); }
static inline double seq_stats_stddev(seq_stats_t* d)   { return f64_sqrt(seq_stats_variance(d)); }

void time_func(void (*f)(void), int32_t n)
{
  m0 = fake_data_0;
  m1 = fake_data_1;
  m2 = fake_data_2;
  
  while(n-- > 0) {
    f();
#if 0    
    m0 = fake_data_0 + (n & (SINK_CHUNKS-1));
    m1 = fake_data_1 + (n & (SINK_CHUNKS-1));
    m2 = fake_data_2 + (n & (SINK_CHUNKS-1));
#endif    
  }
}

int cmp_u64(const void * a, const void * b)
{
  return ( *(uint64_t*)a > *(uint64_t*)b );
}

double timing_run(void (*f)(void), uint64_t data[static time_trials])
{
  struct timespec sleep_req = {0, 100000};
  
  uint32_t s0 = 0;
  double   t;

  do {
    for(uint32_t n=s0; n<time_trials; n++) {
      uint64_t t0 = time_get();
      time_func(f, time_count);
      uint64_t t1 = time_get();
      data[n]     = t1-t0;
      nanosleep(&sleep_req, NULL);
    }
    
    qsort(data, time_trials, sizeof(uint64_t), cmp_u64);
    
    uint64_t dt = data[time_trials-1]-data[0];

    // no,no,no: temp hack
    t = (double)data[0] * time_scale;

    return t;  // kill the re-run thing. I hate timing things
    
    if ((double)dt < (double)data[0] * 1.00005)
      return t;
    
    sleep(0);
    
  } while(++s0 < time_trials);
  
  return t;
}

double timing_gather(seq_stats_t* s, uint64_t data[static time_trials])
{
  seq_stats_init(s);

  for(uint32_t n=0; n<time_trials; n++)
    seq_stats_add(s, (double)data[n] * time_scale);

  // no need to be careful here methinks
  double mean = s->m;
  double sum  = 0.0;

  for(uint32_t n=0; n<time_trials; n++) {
    double v = (double)data[n] * time_scale;
    double d = mean-v;
    sum = fma(d,d,sum);
  }

  return sum/mean;
}

void timing_test(func_entry_t* entry, int len)
{
  //uint32_t rerun = 5;//time_rerun;
  uint64_t data[time_trials];
  seq_stats_t stats;

  printf("┌───────────────────┬"
         "──────────────────────────────┬"
         "────────────────┬"
         "─────────┬"
         "─────────┬"
         "─────────┐"
         "\n");
  
  printf(WARNING "│ %-18s│ %27s   │ %14s │%8s │%8s │%8s │\n" ENDC,
         "function",
         time_string,
         "",
         "min ",
         "median",
         "max "
         );
  
  printf("├───────────────────┼"
         "──────────────────────────────┼"
         "────────────────┼"
         "─────────┼"
         "─────────┼"
         "─────────┤"
         "\n");

  while(len--) {
    printf("│ %-18s", entry->name);

    timing_run(entry->f, data);
    timing_gather(&stats, data);
    
    double std = f64_sqrt(seq_stats_variance(&stats));
    
    printf("│%14f ±%14f│ (1 ± %-8f) │%9.2f│%9.2f│%9.2f│",
           stats.m, std,
           std/stats.m,
           (double)data[0] * time_scale,
           (double)data[time_trials>>1] * time_scale,
           (double)data[time_trials-1] * time_scale
           );
    
    //if (gof > time_vlimit) printf(" <-- garbage timing (retry)");
    printf("\n");

    entry++;
  }

  printf("└───────────────────┴"
         "──────────────────────────────┴"
         "────────────────┴"
         "─────────┴"
         "─────────┴"
         "─────────┘\n");
}

//4) │    8089.56   12876.60   12949.44

// └
//********************************************************


// meh: or not
#if 0//(__GLIBC__ > 2) && (__GLIBC_MINOR__ >= 30)
bool set_thread_affinity(pthread_t thread, int cpu_id)
{
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_id, &cpuset);
  int result = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
  
  return result == 0;
}
#else
bool set_thread_affinity(pthread_t thread, int cpu_id)
{
  (void)(thread);
  (void)(cpu_id);
  return false;
}
#endif

int set_thread_max_priority(pthread_t thread)
{
  struct sched_param param;
  int policy;

  if (pthread_getschedparam(thread, &policy, &param) == 0) {
    policy               = SCHED_FIFO;
    param.sched_priority = sched_get_priority_max(policy);
    
    if (pthread_setschedparam(thread, policy, &param) == 0)
      return true;
  }
  
  return false;
}

int main(void)
{
  prng_t prng;

  prng.state[0] = __rdtsc();
  prng.state[1] = UINT64_C(0x3ba0d900b9aaf028);
  prng.state[2] = UINT64_C(0xab7837b9aa423d86);
  prng_u64(&prng);

  time_string = time_string_ns;
  
  memset(sink_data, 0, sizeof(sink_data));

  for(uint32_t i=0; i<SINK_CHUNKS; i++) {
    fake_data_0[i] = prng_u64(&prng);
    fake_data_1[i] = prng_u64(&prng);
    fake_data_2[i] = prng_u64(&prng);
  }
  
  // doesn't matter if it fails. just a hack
  // when using __rdtsc to have less fails
  // of counter reads (if thread switches
  // and comes back to a different CPU).
  set_thread_affinity(pthread_self(), 0);
  
  if (!set_thread_max_priority(pthread_self()))
    printf("failed to up thread priority\n");
  
  timing_test(func_table,  LENGTHOF(func_table));

  return 0;
}
