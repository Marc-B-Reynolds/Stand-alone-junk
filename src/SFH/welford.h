// Marc B. Reynolds, 2018-2024
// Public Domain under http://unlicense.org, see link for details.

#ifndef WELFORD_H
#define WELFORD_H


// Welford's method for streaming mean/variance/stdev
typedef struct { double n,m,s; } seq_stats_t;

static inline void seq_stats_init(seq_stats_t* d)
{
  memset(d,0,sizeof(seq_stats_t));
}

static inline void seq_stats_add(seq_stats_t* d, float v)
{
  double x = (double)v;
  
  d->n += 1.0;

  double m  = d->m;
  double s  = d->s;
  double dm = x-m;
  
  d->m = m + dm/d->n;
  d->s = s + dm*(x-d->m);  // why I'm I not doing FMA here?
}

static inline double seq_stats_mean(seq_stats_t* d)     { return d->m; }
static inline double seq_stats_variance(seq_stats_t* d) { return d->s/(d->n-1.0); }
static inline double seq_stats_stddev(seq_stats_t* d)   { return sqrt(seq_stats_variance(d)); }

static inline void seq_stats_print(seq_stats_t* d)
{
  printf("mean=%f,variance=%f,std-dev=%f, count=%u\n",
	 d->m,
	 seq_stats_variance(d),
	 seq_stats_stddev(d),
	 (uint32_t)d->n
	 );
}

#endif
