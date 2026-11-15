// -*- coding: utf-8 -*-

// Public Domain under http://unlicense.org, see link for details.

// THIS HAS NO MEANFUL VALUE AT MEASURING PERFORMANCE. Hacky dev pseudo-aid only.
// copy-n-pasted here. no PRNG sampling, no selection of range and it's garbage anyway

//*********************************************************************************************

#if defined(__linux__) || defined(__unix__)
// for thread affinity
#define _GNU_SOURCE 
#endif

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

// this is temp hack garbage
#if defined(__linux__) || defined(__unix__)
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <sys/personality.h>
#elif defined(_WIN32)
#elif defined(__APPLE__) && defined(__MACH__)
#include <unistd.h>
#include <time.h>
#else
#endif



#define PRNG_IMPLEMENTATION
#include "SFH/tests/prng_small_global.h"
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

#ifndef SAMPLE_X0
#define SAMPLE_X0 0.f
#define SAMPLE_WARN
#endif

#ifndef SAMPLE_X1
#define SAMPLE_X1 1.f
#define SAMPLE_WARN
#endif

#ifdef SAMPLE_WARN
#warning "set sampling ranges"
#endif

void time_func(float (*f)(float), int32_t n)
{
  float sink = 0;

  while(n-- > 0) {
    float x = fmaf(((SAMPLE_X1)-(SAMPLE_X0)), prng_f32(), SAMPLE_X0);
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

void timing_test_(func_entry_t* entry, int len)
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


#ifndef ADDR_NO_RANDOMIZE
#define ADDR_NO_RANDOMIZE 0x0040000
#endif

int is_aslr_disabled(void)
{
  int r = personality(0xffffffff);
  
  if (r == -1) {
    fprintf(stderr, "error: couldn't query personality\n");
    return -1;
  }

  return (r & ADDR_NO_RANDOMIZE) != 0;
}


void timing_test(func_entry_t* entry, int len)
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
    printf("thread priority: default (did you: sudo setarch -R)\n");
  else
    printf("thread priority: realtime\n");

  if (is_aslr_disabled())
    printf("ASLR:            disabled\n");
  else
    printf("ASLR:            enabled\n");

  printf("uniform samples: [%f,%f]\n", SAMPLE_X0,SAMPLE_X1);
  timing_test_(entry,len);
}

