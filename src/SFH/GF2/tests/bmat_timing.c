
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define _GNU_SOURCE
#define __USE_GNU
#include <errno.h> // sigh
#include <pthread.h>
#include <sched.h>

#include "../bmat_everything.h"


#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))

uint64_t sink_data[1024];   // temp hack
uint64_t fake_data_0[1024]; // temp hack
uint64_t fake_data_1[1024]; // temp hack
uint64_t fake_data_2[1024]; // temp hack

uint64_t* m0;
uint64_t* m1;
uint64_t* m2;

#define STRINGIFY(S)  STRINGIFY_(S)
#define STRINGIFY_(S) #S

//--------------------------------------------------

// f(array,array) calls
#define FOR_2M_FUNCS(DO) \
    DO(transpose_8)      \
    DO(transpose_16)     \
    DO(transpose_32)	 \
    DO(transpose_64)

#define FOR_2R_FUNCS(DO) \
    DO(transpose_8_ref)  \
    DO(transpose_16_ref) \
    DO(transpose_32_ref) \
    DO(transpose_64_ref)

// f(array,array,array) calls
#define FOR_3M_FUNCS(DO) \
    DO(mul_8)            \
    DO(mul_16)           \
    DO(mul_32)	         \
    DO(mul_64)           \
    DO(mult_8)           \
    DO(mult_16)          \
    DO(mult_32)	         \
    DO(mult_64)

#define FOR_3R_FUNCS(DO) \
    DO(mul_8_ref)        \
    DO(mul_16_ref)       \
    DO(mul_32_ref)	 \
    DO(mul_64_ref)       \
    DO(mult_8_ref)       \
    DO(mult_16_ref)      \
    DO(mult_32_ref)	 \
    DO(mult_64_ref)


//--------------------------------------------------

#define WRAP_2M(N) void N(void) { bmat_##N(m0,m1); }
#define WRAP_3M(N) void N(void) { bmat_##N(m0,m1,m2); }

// expand wrappers for f(matrix,matrix) calls
FOR_2M_FUNCS(WRAP_2M)
FOR_2R_FUNCS(WRAP_2M)
FOR_3M_FUNCS(WRAP_3M)
FOR_3R_FUNCS(WRAP_3M)

typedef struct {
  void (*f)(void);
  char*   name;
} func_entry_t;

     
#define ENTRY(X) { .f=&X, .name=STRINGIFY(X) },
     
func_entry_t func_table[] =
{
//FOR_2R_FUNCS(ENTRY)
  FOR_2M_FUNCS(ENTRY)
  FOR_3R_FUNCS(ENTRY)
  FOR_3M_FUNCS(ENTRY)
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

const double time_scale = 1.0/((double)time_count);

static inline uint64_t time_get(void)
{
#if 1
  #define TIME_STR "cycles"
  return _rdtsc();
#else  
  #define TIME_STR "ns"
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (1000*1000*1000*(uint64_t)ts.tv_sec + (uint64_t)ts.tv_nsec);
#endif  
}

static inline double doubletime(void)
{
  return (double)time_get();
}

// Welford's method for streaming mean/variance/stdev
// --- just being lazy
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
  
  while(n-- > 0) { f(); m0 += 0; m1 += 0; m2 += 0;}
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
	 "────────────────┐\n");
  printf("│ %-18s│ %27s   │ %14s │\n", "function", "mean ± std (cycles) ","");
  printf("├───────────────────┼"
	 "──────────────────────────────┼"
	 "────────────────┤\n");

  while(len--) {
    printf("│ %-18s", entry->name);

    timing_run(entry->f, data);
    timing_gather(&stats, data);
    
    double std = f64_sqrt(seq_stats_variance(&stats));
    
    printf("│%14f ±%14f│ (1 ± %-8f) │%8.2f│%8.2f│%8.2f│",
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
	 "────────────────┘\n");
}

//4) │    8089.56   12876.60   12949.44

// └
//********************************************************


#if (__GLIBC__ > 2) && (__GLIBC_MINOR__ >= 30)
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

  memset(sink_data, 0, sizeof(sink_data));

  // doesn't matter if it fails. just a hack
  // using __rdtsc (complete comment..not
  // thinking of wording)
  set_thread_affinity(pthread_self(), 0);
  
  if (!set_thread_max_priority(pthread_self()))
    printf("failed to up thread priority\n");
  
  timing_test(func_table,  LENGTHOF(func_table));

  return 0;
}
