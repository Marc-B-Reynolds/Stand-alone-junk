// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2001-2025
// Public Domain under http://unlicense.org, see link for details.

//#pragma once

#define SFH_MACRO_H    // marker define

//*******************************************************
// common defines

#define SFH_PARENS ()


//*******************************************************
// flattening variants

// remove the parentheses around the "single" input parameter.
// produces an error if it isn't.
//
//   SFH_SIMPLE_FLATTEN((a))   → a
//   SFH_SIMPLE_FLATTEN((a,b)) → a,b
#define SFH_SIMPLE_FLATTEN(x) SFH_SIMPLE_FLATTEN_ x
#define SFH_SIMPLE_FLATTEN_(...) __VA_ARGS__

//*******************************************************

// rotate macro parameters left:
//   ()          → nil
//   (1)         → 1
//   (1,2,...,N) → 2,...,N,1
#define SFH_ROL(...)    SFH_ROL_(__VA_ARGS__)
#define SFH_ROL_(D,...) __VA_OPT__(__VA_ARGS__,)D

#define SFH_ROL2(...)   SFH_ROL_(__VA_ARGS__)
#define SFH_ROL2_(...)  SFH_ROL(SFH_ROL(__VA_ARGS__))

// first parameter (Take[1,list])
//   ()      → nil
//   (1,...) → 1
#define SFH_TAKE(...)    SFH_TAKE_(__VA_ARGS__)
#define SFH_TAKE_(N,...) N


// these require at least as long as the specified length
#define SFH_TAKE_2(_1,_2,x,...) x
#define SFH_TAKE_3(_1,_2,_3,x,...) x
#define SFH_TAKE_4(_1,_2,_3,_4,x,...) x
#define SFH_TAKE_5(_1,_2,_3,_4,_5,x,...) x
#define SFH_TAKE_6(_1,_2,_3,_4,_5,_6,x,...) x
#define SFH_TAKE_7(_1,_2,_3,_4,_5,_6,_7,x,...) x
#define SFH_TAKE_8(_1,_2,_3,_4,_5,_6,_7,_8,x,...) x
#define SFH_TAKE_9(_1,_2,_3,_4,_5,_6,_7,_8,_9,x,...) x

// for argcount based selection expansions
#define SFH_ARGCOUNT_8(...) SFH_TAKE_8(__VA_OPT__(__VA_ARGS__,) 8,7,6,5,4,3,2,1,0)


// drop first parameter (Drop[1,list])
//   ()      → nil
//   (1)     → nil
//   (1,...) → ...
#define SFH_DROP(...)    __VA_OPT__(SFH_DROP_(__VA_ARGS__))
#define SFH_DROP_(N,...) __VA_ARGS__

// append/prepend 
//   (X)     → X         
//   (X,...) → ...,X  (append)
//   (X,...) → X,...  (prepend)
#define SFH_APPEND(X,...)      __VA_OPT__(__VA_ARGS__,)X
#define SFH_PREPEND(X,...)     X __VA_OPT__(,__VA_ARGS__)


//*******************************************************
// David Mazières' modernized X macro
// https://www.scs.stanford.edu/~dm/blog/va-opt.html

#define SFH_EXPAND(...)  SFH_EXPAND4(SFH_EXPAND4(SFH_EXPAND4(SFH_EXPAND4(__VA_ARGS__))))
#define SFH_EXPAND4(...) SFH_EXPAND3(SFH_EXPAND3(SFH_EXPAND3(SFH_EXPAND3(__VA_ARGS__))))
#define SFH_EXPAND3(...) SFH_EXPAND2(SFH_EXPAND2(SFH_EXPAND2(SFH_EXPAND2(__VA_ARGS__))))
#define SFH_EXPAND2(...) SFH_EXPAND1(SFH_EXPAND1(SFH_EXPAND1(SFH_EXPAND1(__VA_ARGS__))))
#define SFH_EXPAND1(...) __VA_ARGS__

// SFH_MAP(F,...)
//   map `macro` across all listed varargs (...)
//
// SFH_MAP(F,0,1,2,...,N) → F(0) F(1) F(2) .. F(N)
//
// SFH_SMAP performs same but for lists limited to 8 or less
#define SFH_MAP(macro, ...) \
  __VA_OPT__(SFH_EXPAND(SFH_MAP_HELPER(macro, __VA_ARGS__)))

#define SFH_MAP_HELPER(macro, a1, ...)                     \
  macro(a1)                                                \
  __VA_OPT__(SFH_MAP_AGAIN SFH_PARENS (macro, __VA_ARGS__))

#define SFH_MAP_AGAIN() SFH_MAP_HELPER


// SFH_MAP_PEEL(F,P,...)
//   maps macro 'F' across the vararg list where P
//   is one or more additional parameters (enclosed in parens)
//
// examples of passing one and two:
//   SFH_SMAP_PEEL(F,(A),0,1,...)   → F(A,0) F(A,1) ...
//   SFH_SMAP_PEEL(F,(A,B),0,1,...) → F(A,B,0) F(A,B,1) ...
//
// SFH_SMAP_PEEL performs same but for lists limited to 8 or less

#define SFH_MAP_PEEL(macro, ...)                                  \
  __VA_OPT__(SFH_EXPAND(SFH_MAP_PEEL_(macro,__VA_ARGS__)))

#define SFH_MAP_PEEL_(macro,P,P1, ...)                            \
  macro(SFH_SIMPLE_FLATTEN(P),P1)                                 \
  __VA_OPT__(SFH_MAP_PEEL_REP SFH_PARENS (macro,P,__VA_ARGS__))

#define SFH_MAP_PEEL_REP() SFH_MAP_PEEL_



//*******************************************************
// A non-rescan expansion (argcount based) limited up to 8 
// varargs. Lighter weight preprocessing time than SFH_MAP.

// SFH_SMAP(F,...)
//
// Like SFH_MAP just limited to max list of 8

#define SFH_SMAP1(F,A)               F(A)
#define SFH_SMAP2(F,A,B)             F(A) F(B)
#define SFH_SMAP3(F,A,B,C)           F(A) F(B) F(C)
#define SFH_SMAP4(F,A,B,C,D)         F(A) F(B) F(C) F(D)
#define SFH_SMAP5(F,A,B,C,D,E)       F(A) F(B) F(C) F(D) F(E)
#define SFH_SMAP6(F,A,B,C,D,E,G)     F(A) F(B) F(C) F(D) F(E) F(G)
#define SFH_SMAP7(F,A,B,C,D,E,G,H)   F(A) F(B) F(C) F(D) F(E) F(G) F(H)
#define SFH_SMAP8(F,A,B,C,D,E,G,H,I) F(A) F(B) F(C) F(D) F(E) F(G) F(H) F(I)

#define SFH_SMAP(F,...)    SFH_SMAP_(F, SFH_ARGCOUNT_8(__VA_ARGS__), __VA_ARGS__)
#define SFH_SMAP_(F,N,...) SFH_CAT2(SFH_SMAP,N)(F,__VA_ARGS__)

// SFH_SMAP_PEEL(F,X,...)
// maps macro 'F' across the vararg list (up to 8 long) where X
// is one or more additional parameters enclosed in parens.
//
// examples of passing one and two:
//   SFH_SMAP_PEEL(F,(A),0,1,...)   → F(A,0) F(A,1) ...
//   SFH_SMAP_PEEL(F,(A,B),0,1,...) → F(A,B,0) F(A,B,1) ...

#define SFH_SMAP_PEEL1(F,X,A)               F(X,A)
#define SFH_SMAP_PEEL2(F,X,A,B)             F(X,A) F(X,B)
#define SFH_SMAP_PEEL3(F,X,A,B,C)           F(X,A) F(X,B) F(X,C)
#define SFH_SMAP_PEEL4(F,X,A,B,C,D)         F(X,A) F(X,B) F(X,C) F(X,D)
#define SFH_SMAP_PEEL5(F,X,A,B,C,D,E)       F(X,A) F(X,B) F(X,C) F(X,D) F(X,E)
#define SFH_SMAP_PEEL6(F,X,A,B,C,D,E,G)     F(X,A) F(X,B) F(X,C) F(X,D) F(X,E) F(X,G)
#define SFH_SMAP_PEEL7(F,X,A,B,C,D,E,G,H)   F(X,A) F(X,B) F(X,C) F(X,D) F(X,E) F(X,G) F(X,H)
#define SFH_SMAP_PEEL8(F,X,A,B,C,D,E,G,H,I) F(X,A) F(X,B) F(X,C) F(X,D) F(X,E) F(X,G) F(X,H) F(X,I)

#define SFH_SMAP_PEEL(F,X,...)    SFH_SMAP_PEEL_(F,X, SFH_ARGCOUNT_8(__VA_ARGS__), __VA_ARGS__)
#define SFH_SMAP_PEEL_(F,X,N,...) SFH_CAT2(SFH_SMAP_PEEL,N)(F,SFH_SIMPLE_FLATTEN(X),__VA_ARGS__)


//*******************************************************
// argument count based concat (limited to 8 for one, nest for more)

#define SFH_CAT0()
#define SFH_CAT1(A) A
#define SFH_CAT2(A,B) A##B
#define SFH_CAT3(A,B,C) A##B##C
#define SFH_CAT4(A,B,C,D) A##B##C##D
#define SFH_CAT5(A,B,C,D,E) A##B##C##D##E
#define SFH_CAT6(A,B,C,D,E,F) A##B##C##D##E##F
#define SFH_CAT7(A,B,C,D,E,F,G) A##B##C##D##E##F##G
#define SFH_CAT8(A,B,C,D,E,F,G,H) A##B##C##D##E##F##G##H

#define SFH_CAT_(_0,_1,_2,_3,_4,_5,_6,_7,M,...) M
#define SFH_CAT(...) SFH_TAKE_8(__VA_ARGS__,SFH_CAT8,SFH_CAT7,SFH_CAT6,SFH_CAT5,SFH_CAT4,SFH_CAT3,SFH_CAT2,SFH_CAT1,SFH_CAT0)(__VA_ARGS__)


//*******************************************************
// argcount base default parameters

// map to a single macro or C call
//
// different target name example: 
//   #define foo(...) SFH_DEF_PARAM_3(foo_,2,3,__VA_ARGS__)
//
//   foo()        → foo_(,2,3)     { illegal }
//   foo(a)       → foo_(a,2,3)
//   foo(a,b)     → foo_(a,b,3)
//   foo(a,b,c)   → foo_(a,b,c)
//   foo(a,b,c,d) → d              { illegal (or any longer list maps to this) }
//
// if target is a C call than can use the same name
//   #define foo(...) SFH_DEF_PARAM_3(foo,2,3,__VA_ARGS__)

#define SFH_DEF_PARAM_2(F,D2,...)       SFH_TAKE_2(__VA_ARGS__, F(__VA_ARGS__), F(__VA_ARGS__,D2),)
#define SFH_DEF_PARAM_3(F,D2,D3,...)    SFH_TAKE_3(__VA_ARGS__, F(__VA_ARGS__), F(__VA_ARGS__,D2), F(__VA_ARGS__,D2,D3),)
#define SFH_DEF_PARAM_4(F,D2,D3,D4,...) SFH_TAKE_4(__VA_ARGS__, F(__VA_ARGS__), F(__VA_ARGS__,D2), F(__VA_ARGS__,D2,D3), F(__VA_ARGS__,D2,D3,D4),)


// map the supplied # of parameters to a unique macro or C call (up to 8)
//   #define foo(...) SFH_PARAM_MAP(foo,__VA_ARGS__)
//
//   foo()      → foo_0()
//   foo(a)     → foo_1(a)
//   foo(a,b)   → foo_2(a,b)
//   foo(a,b,c) → foo_3(a,b,c)

#define SFH_PARAM_MAP(F,...)    SFH_PARAM_MAP_(F, SFH_ARGCOUNT_8(__VA_ARGS__), __VA_ARGS__)
#define SFH_PARAM_MAP_(F,N,...) SFH_CAT2(F##_,N)(__VA_ARGS__)

