// Public Domain under http://unlicense.org, see link for details.

// THIS HAS NO MEANFUL VALUE AT MEASURING PERFORMANCE. Hacky dev pseudo-aid only.
// copy-n-pasted here. no PRNG sampling, no selection of range and it's garbage anyway

//*********************************************************************************************

const uint32_t time_count  = 1<<20;   // number of func calls/trial
const  int32_t time_trials =    16;   // number of trials
const  int32_t time_retry  =    15;   // max number of retries
const  int32_t time_rerun  =     0;   // max number of retries
const   double time_vlimit =  0.02;   // stdd acceptable value (or retry)

const double time_scale = 1.0/((double)time_count);

static inline uint64_t time_get(void)
{
#if 1
  #define TIME_STR "cycles"
  return _rdtsc();
#else  
  #define TIME_STR "ns    "
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

inline void seq_stats_init(seq_stats_t* d)
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

inline double seq_stats_mean(seq_stats_t* d)     { return d->m; }
inline double seq_stats_variance(seq_stats_t* d) { return d->s/(d->n-1.0); }
inline double seq_stats_stddev(seq_stats_t* d)   { return sqrt(seq_stats_variance(d)); }

float time_func(float (*f)(float), int32_t n)
{
  // use an additive recurrence to cover the range cheaply
  // currently: [0,1]...which is useless here...copy-n-paste...needs the fixing part
  // WAS WRITTEN FOR BRANCH FREE. needs different strategy
  // for branchy functions...cheap PRNG
  static const uint32_t A = 2654435769;

  float    r = 0.f;
  float    x = 0.f;
  uint32_t u = A;
  float    v;
  
  while(n-- > 0) {
    v  = f(x);

    if (v != v) { printf("%f ", x); }

    u += A;
    x  = (float)(u) * (0x1.0p-33f);
    r += v;

    if (r != r) { printf("%f ", v); }
  }
  
  return r;
}

int cmp_u64(const void * a, const void * b)
{
  return ( *(uint64_t*)a > *(uint64_t*)b );
}

volatile float sink;

void timing_run(float (*f)(float), uint64_t data[static time_trials])
{
  uint32_t s0 = 0;

  do {
    for(uint32_t n=s0; n<time_trials; n++) {
      uint64_t t0 = time_get();
      float    r  = time_func(f, time_count);
      uint64_t t1 = time_get();
      data[n]     = t1-t0;
      sink       += r;
    }
    
    qsort(data, time_trials, sizeof(uint64_t), cmp_u64);
    
    uint64_t dt = data[time_trials-1]-data[0];

    return;  // kill the re-run thing. I hate timing things
    // intended to do: kill outliners and give up some
    // CPU time if needed and whatnot. meh. do nothing
    // throughput measures are meh.
    
    if ((double)dt < (double)data[0] * 1.00005)
      return;
    
    sleep(0);
    
  } while(++s0 < time_trials);
}

double timing_gather(seq_stats_t* s, uint64_t data[static time_trials])
{
  seq_stats_init(s);

  for(uint32_t n=0; n<time_trials; n++)
    seq_stats_add(s, (double)data[n] * time_scale);

  // no need to be careful here methinks
  // garbage.
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
  double gof = 0.0;

  printf("%15s %10s %10s\n", "function", "ave "  TIME_STR, "std");
  printf("-------------------------------------------------------\n");
  
  while(len--) {
    
    printf("%15s ", entry->name);
    
    // temp hack (variable)
    timing_run(entry->f, data);
    gof = timing_gather(&stats, data);
    
    double std = sqrt(seq_stats_variance(&stats));
    
    printf("%12f %10f %10f",stats.m, std, gof);
    
    //if (gof > time_vlimit) printf(" <-- garbage timing (retry)");
    printf("\n");

    entry++;
  }
    
  printf("sink = %f\n", sink);
}

