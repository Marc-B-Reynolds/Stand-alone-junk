// -*- coding: utf-8 -*-

// for thread affinity
#define _GNU_SOURCE 

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
//#include <errno.h> // sigh
#include <pthread.h>
#include <sched.h>

#include "SFH/compiler_hints.h"
#include "SFH/bitops.h"
#include "SFH/f32_util.h"
#include "SFH/f64_util.h"
#include "SFH/f32_horner.h"

#include "SFH/f32_math/internal/f32_asincos.h"

static inline float asin_wrap(float x, float (*f)(float))
{
  float x2 = x*x;
  float r  = x2*f(x2);
  
  return fmaf(r,x,x);
}


static inline float asin_hi_d0(float a)
{
  static const double N[] =  {
     0x1.8bac461c72755p-7,
    -0x1.f3ac5d6a5b43p-5,
     0x1.a188f1a4fe77dp-3,
    -0x1.91b45060ec63bp0
  };
  
  double x = (double)a;
  double s = sqrt(1.0-x);
  double p = N[0];

  p = fma(p,x,N[1]);
  p = fma(p,x,N[2]);
  p = fma(p,x,N[3]);
#if 0
  p = fma(p,s, 0.5*f64_pi);
#elif 1
  double h = p*s;
  double l = fma(p,s,-h);

  p = 0.5*f64_up_pi.h+(h + (l + 0.5*f64_up_pi.l));
#else
  p = fma(p,s, 0.5*f64_up_pi.l) + 0.5*f64_up_pi.h;
#endif  

  return (float)p;
}


static inline float asin_bf(float a)
{
  static const double N[] =  {
    // polynomial for |x| on [0, 1/2]
    0x1.596d288dc7987p-5f,
    0x1.8c283c3a5a46ap-6f,
    0x1.747e4a3065be5p-5f,
    0x1.3301e4689933p-4f,
    0x1.5555c88340c2cp-3f,

    // polynomial for |x| on [1/2, 1]
    -0x1.34df4625198ddp-8,
     0x1.a354224d7d72p-6,
    -0x1.34625edcedc4ap-4,
     0x1.af0d71c21deefp-3,
    -0x1.91fdeaf5921aap0
  };

  double   x  = (double)a;
  uint64_t sx = f64_sign_bit(x);

  x = f64_mulsign(x,sx);

#if 1
  int      i  = (x <= 0.5);
  double   v  = hint_select(i, x*x, x);
  double   n  = hint_select(i, v,   1.0);
  double   b  = hint_select(i, x,   0.5*f64_pi);
  double   s  = hint_select(i, x,   sqrt(1.0-x));
  const double*  c  = hint_select(i, N, N+5);
#elif 0
  uint64_t m  = f64_sign_mask(x-0.5);
  double   v  = f64_mask_select(m, x*x, x);
  double   n  = f64_mask_select(m, v,   1.0);
  double   b  = f64_mask_select(m, x,   0.5*f64_pi);
  double   s  = f64_mask_select(m, x,   sqrt(1.0-x));
  const double*  c  = N + ((~m) & 5);
#else
  double   v  = (x <= 0.5) ? x*x : x;
  double   n  = (x <= 0.5) ? v   : 1.0;
  double   b  = (x <= 0.5) ? x   : 0.5*f64_pi;
  double   s  = (x <= 0.5) ? x   : sqrt(1.0-x);
  const double*  c  = (x <= 0.5) ? N   : N+5;
#endif  
  
  double   r  = c[0];

  r = fma(r, v, c[1]);
  r = fma(r, v, c[2]);
  r = fma(r, v, c[3]);
  r = fma(r, v, c[4]) * n;
  r = fma(r, s, b);

  return (float)f64_mulsign(r,sx);
}


static inline float asin_d_hi_fr(float a)
{
  static const double N[] =  {
    // polynomial on [1/2, 1]
    -0x1.34df4625198ddp-8,
     0x1.a354224d7d72p-6,
    -0x1.34625edcedc4ap-4,
     0x1.af0d71c21deefp-3,
    -0x1.91fdeaf5921aap0
  };
  
  double   x  = (double)a;
  uint64_t sx = f64_sign_bit(x);

  x = f64_mulsign(x,sx);

  double s = sqrt(1.0-x);
  double r = N[0];

  double v = x;

  r = fma(r, v, N[1]);
  r = fma(r, v, N[2]);
  r = fma(r, v, N[3]);
  r = fma(r, v, N[4]);

  r = fma(r,s, 0.5*f64_pi);

  return (float)f64_mulsign(r,sx);
}

#if 0
float asin_r2(float x) { return asin_wrap(x,f32_asincos_r2); }
float asin_k4(float x) { return asin_wrap(x,f32_asincos_k4); }

#define X0 -0.5f
#define X1  0.5f

#else
float asin_r2(float x) { return f32_asin_x1(x,f32_asincos_r2); }
float asin_k4(float x) { return f32_asin_x1(x,f32_asincos_k4); }

#define X0  0.5f
#define X1  1.f
#endif



//────────────────────────────────────────────────────────────────────────────────────

#define LENGTHOF(X) (sizeof(X)/sizeof(X[0]))

#define STRINGIFY(S)  STRINGIFY_(S)
#define STRINGIFY_(S) #S


typedef struct {
  float (*f)(float);
  char*   name;
} func_entry_t;

     
#define ENTRY(X) { .f=X, .name=STRINGIFY(X) }
     
func_entry_t func_table[] =
{
  ENTRY(asinf),
  ENTRY(asin_r2),
  ENTRY(asin_k4),
  ENTRY(asin_bf),
  ENTRY(asin_d_hi_fr),
  //ENTRY(f32_atan),
  //ENTRY(f32_atanpi_fr),
  //ENTRY(f32_atanpi_ue),
  //ENTRY(f32_atanpi_ue_a),
  //ENTRY(f32_atanpi_ue_2),
  //ENTRY(f32_atan_ue_p),
  //ENTRY(f32_atan_ue),
  //ENTRY(asinf),
  //ENTRY(fdlibm_asinf),
};


//────────────────────────────────────────────────────────────────────────────────────

#define PRNG_IMPLEMENTATION
#include "prng_small_global.h"
#include "SFH/welford.h"

#include <time.h>

#if 1
const uint32_t time_count  = 1<<18;   // number of func calls/trial
const  int32_t time_trials =    64;   // number of trials
#else
const uint32_t time_count  = 1<<16;   // number of func calls/trial
const  int32_t time_trials =    32;   // number of trials
#endif

const  int32_t time_retry  =    15;   // max number of retries
const  int32_t time_rerun  =     0;   // max number of retries
const   double time_rlimit =  0.10;   // mean/std threshold (meh)

// 
char*  time_string_cycles = "mean ± std (cycles) ";
char*  time_string_ns     = "  mean ± std (ns)   ";
bool   time_cycles = true;
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

volatile float global_sink = 0;

#if defined(__GNUC__) || defined(__clang__)
#if defined(__arm64__) || defined(__aarch64__)


#define HINT_ASMC_F "w"
#else
#define HINT_ASMC_F "x"
#endif
#define HINT_ASMC_I "r"
#endif

#define asm_volatile __asm__ __volatile__
#define hint_alive_fp(V)  ({ typeof(V) _v = (V); asm_volatile("" : "+" HINT_ASMC_F (_v)); _v; })

void time_func(float (*f)(float), int32_t n)
{
  float sink = 0;

  while(n-- > 0) {
    float x = fmaf(((X1)-(X0)), prng_f32(), X0);
    sink += f(x);
  }

  global_sink += sink;
}

int cmp_u64(const void * a, const void * b)
{
  return ( *(uint64_t*)a > *(uint64_t*)b );
}

double timing_run(float (*f)(float), uint64_t data[static time_trials])
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
    seq_stats_add(s, (float)data[n] * (float)time_scale);

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

// massive temp hack
#define HEADER     "\033[95m"
#define OKBLUE     "\033[94m"
#define OKCYAN     "\033[96m"
#define OKGREEN    "\033[92m"
#define WARNING    "\033[93m"
#define FAIL       "\033[91m"
#define ENDC       "\033[0m"
#define BOLD       "\033[1m"
#define UNDERLINE  "\033[4m"

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
    
    double std = sqrt(seq_stats_variance(&stats));
    
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

// meh: or not
#if 1//(__GLIBC__ > 2) && (__GLIBC_MINOR__ >= 30)
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
  printf("thread affinity: (compile time disabled)\n");
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

void timing_hack(void)
{
  if (time_cycles)
    time_string = time_string_cycles;
  else
    time_string = time_string_ns;
    
  
  // doesn't matter if it fails. just a hack
  // when using __rdtsc to have less fails
  // of counter reads (if thread switches
  // and comes back to a different CPU).
  if (set_thread_affinity(pthread_self(), 0))
    printf("thread affinity: CPU 0\n");
  
  // if fails probably forgot to sudo
  if (!set_thread_max_priority(pthread_self()))
    printf("thread priority: default (did you sudo?)\n");
  else
    printf("thread priority: realtime\n");


  printf("uniform samples: [%f,%f]\n", X0,X1);
  timing_test(func_table,  LENGTHOF(func_table));
}

int main(void)
{
  timing_hack();
  return 0;
}
