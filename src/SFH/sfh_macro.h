// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2001-2026
// Public Domain under http://unlicense.org, see link for details.

#pragma once

#define SFH_MACRO_H    // marker define

//#if defineddefined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL
//#endif

#define SFH_STRINGIFY_(x) #x
#define SFH_STRINGIFY(x)  SFH_STRINGIFY_(x)

//────────────────────────────────────────────────────────────────────────────────────
// common defines

#define SFH_LENGTHOF(A) (sizeof(A)/sizeof(A[0]))


// allow selecting the default number of rescans. See
// SFH_EXPAND_P{n} description below.
#ifndef SFH_EXPAND
// current default is 512
#define SFH_EXPAND SFH_EXPAND_P9
#endif


#define SFH_PARENS ()
#define SFH_EVAL(...) __VA_ARGS__

//────────────────────────────────────────────────────────────────────────────────────
// flattening variants

// remove the parentheses around the "single" input parameter.
// produces an error if it isn't.
//
//   SFH_SIMPLE_FLATTEN()      → nil
//   SFH_SIMPLE_FLATTEN(())    → nil
//   SFH_SIMPLE_FLATTEN((a))   → a
//   SFH_SIMPLE_FLATTEN((a,b)) → a,b
//
//   SFH_SIMPLE_FLATTEN(a)     → SFH_SIMPLE_FLATTEN_ a  { illegal }

#define SFH_SIMPLE_FLATTEN(...) __VA_OPT__(SFH_SIMPLE_FLATTEN_ __VA_ARGS__)
#define SFH_SIMPLE_FLATTEN_(...) __VA_ARGS__

//────────────────────────────────────────────────────────────────────────────────────
// rotate macro parameters left:
//   ()          → nil
//   (1)         → 1
//   (1,2,...,N) → 2,...,N,1
#define SFH_ROL(...)    SFH_ROL_(__VA_ARGS__)
#define SFH_ROL_(D,...) __VA_OPT__(__VA_ARGS__,)D

// first parameter (Take[1,list])
//   ()      → nil
//   (1,...) → 1
#define SFH_TAKE(...)    SFH_TAKE_(__VA_ARGS__)
#define SFH_TAKE_(N,...) N

// drop first parameter (Drop[1,list])
//   ()      → nil
//   (1)     → nil
//   (1,...) → ...
#define SFH_DROP(...)    __VA_OPT__(SFH_DROP_(__VA_ARGS__))
#define SFH_DROP_(N,...) __VA_ARGS__

// append/prepend 
//   ()      → nil
//   (X)     → X         
//   (X,...) → ...,X  (append)
//   (X,...) → X,...  (prepend)
#define SFH_APPEND(...)       __VA_OPT__(SFH_APPEND_(__VA_ARGS__))
#define SFH_PREPEND(...)      __VA_OPT__(SFH_PREPEND_(__VA_ARGS__))
#define SFH_APPEND_(X,...)    __VA_OPT__(__VA_ARGS__,) X
#define SFH_PREPEND_(X,...) X __VA_OPT__(,__VA_ARGS__)



//════════════════════════════════════════════════════════════════════════════════════
//
// The macros in this section use rescanning. The maximum number of parameters
// is 65536. The default max is 2^p where p is defined by how SFH_EXPAND
// is defined (default is 512). For either shorter or longer than the default there
// are _X variants of the macros that allow choosing this value (see below). Shorter
// is to cut compile times. Example: these all expand as described when the
// comment constraint is met:
//                                      parameter list length
//     SFH_MAP(F,...)                   ≤ default 
//     SFH_MAP_X(F,SFH_EXPAND_P16,...)  ≤ 65536
//     SFH_MAP_X(F,SFH_EXPAND_P8,...)   ≤ 256
//
// When the parameter list is a small number there are argcount based version later
// which perform no rescanning at all. Both of these reduced max parameter methods
// are because these rescan macros minimally require as many expansion steps as
// the max supported. So a single parameter with max support = 512 will require
// the CPP to perform more than 512 expansions. An argcount based macro will take
// a small fixed number (like 7).
//
//────────────────────────────────────────────────────────────────────────────────────
// Paul Fultz's rescan
// https://github.com/pfultz2/Cloak/wiki/Is-the-C-preprocessor-Turing-complete%3F
//
// rescans: c = count/expansion, n = number of expansions. 
//   f(c,1) = c+1
//   f(c,n) = c*f(c,n-1)+1 = Sum[i,0,n] c^i = (c^(n+1)-1)/(c-1)
// total rescans = f(c,n)+1
//
// BUT! Let c=2 then: f(2,n)+1 = 2^(n+1)
//
// SFH_EXPAND_P{n} expands 2^n times.  By addition chains (of the exponent) we can
// construct any length. Trivial example: 12 = 2^3 + 2^2 so a specialized expansion
// that handles lists up to 12 long is:
// 
//   #define SFH_EXPAND_12(...) SFH_EXPAND_P3(SFH_EXPAND_P2(__VA_ARGS__))
//
// SEE: _X macros comment in at the top of this comment section
// 

#define SFH_EXPAND_P16(...) SFH_EXPAND_P15(SFH_EXPAND_P15(__VA_ARGS__))
#define SFH_EXPAND_P15(...) SFH_EXPAND_P14(SFH_EXPAND_P14(__VA_ARGS__))
#define SFH_EXPAND_P14(...) SFH_EXPAND_P13(SFH_EXPAND_P13(__VA_ARGS__))
#define SFH_EXPAND_P13(...) SFH_EXPAND_P12(SFH_EXPAND_P12(__VA_ARGS__))
#define SFH_EXPAND_P12(...) SFH_EXPAND_P11(SFH_EXPAND_P11(__VA_ARGS__))
#define SFH_EXPAND_P11(...) SFH_EXPAND_P10(SFH_EXPAND_P10(__VA_ARGS__))
#define SFH_EXPAND_P10(...) SFH_EXPAND_P9(SFH_EXPAND_P9(__VA_ARGS__))
#define SFH_EXPAND_P9(...)  SFH_EXPAND_P8(SFH_EXPAND_P8(__VA_ARGS__))
#define SFH_EXPAND_P8(...)  SFH_EXPAND_P7(SFH_EXPAND_P7(__VA_ARGS__))
#define SFH_EXPAND_P7(...)  SFH_EXPAND_P6(SFH_EXPAND_P6(__VA_ARGS__))
#define SFH_EXPAND_P6(...)  SFH_EXPAND_P5(SFH_EXPAND_P5(__VA_ARGS__))
#define SFH_EXPAND_P5(...)  SFH_EXPAND_P4(SFH_EXPAND_P4(__VA_ARGS__))
#define SFH_EXPAND_P4(...)  SFH_EXPAND_P3(SFH_EXPAND_P3(__VA_ARGS__))
#define SFH_EXPAND_P3(...)  SFH_EXPAND_P2(SFH_EXPAND_P2(__VA_ARGS__))
#define SFH_EXPAND_P2(...)  SFH_EXPAND_P1(SFH_EXPAND_P1(__VA_ARGS__))
#define SFH_EXPAND_P1(...) __VA_ARGS__


// David Mazières' modernized X macro
// https://www.scs.stanford.edu/~dm/blog/va-opt.html
//
// SFH_MAP(F,...)
//   map `macro` across all listed varargs (...)
//
// SFH_MAP(F,0,1,2,...,N) → F(0) F(1) F(2) ... F(N)

#define SFH_MAP_X(macro,rescan,...) \
  __VA_OPT__(rescan(SFH_MAP_(macro, __VA_ARGS__)))
#define SFH_MAP(macro,...) SFH_MAP_X(macro,SFH_EXPAND,__VA_ARGS__)
#define SFH_MAP_REP() SFH_MAP_

#define SFH_MAP_(macro, a1, ...)                           \
  macro(a1)                                                \
  __VA_OPT__(SFH_MAP_REP SFH_PARENS (macro, __VA_ARGS__))



// SFH_MAP_LIST(F,...)
//   map `macro` across all listed varargs (...) forming a comma
//   seperated list. The same can be performed with SFH_MAP except
//   it will contain a trailing comma. If this is an issue.
//
//   #define G(X) F(X),
//   SFH_MAP(G,0,1,2,...,N)      → F(0),F(1),F(2),...F(N),
//   SFH_MAP_LIST(F,0,1,2,...,N) → F(0),F(1),F(2),...F(N)

// copy-pasta duplicate of MAP except macro expansion places a comma
// in front. The tail-end of the macro simply drops the "empty" parameter.
#define SFH_MAP_LIST_X(F,rescan,...) SFH_DROP(__VA_OPT__(rescan(SFH_MAP_LIST_(F, __VA_ARGS__))))
#define SFH_MAP_LIST(F,...) SFH_MAP_LIST_X(F,SFH_EXPAND,__VA_ARGS__)
#define SFH_MAP_LIST_REP() SFH_MAP_LIST_

#define SFH_MAP_LIST_(F, a1, ...)                                   \
  ,F(a1)                                                            \
  __VA_OPT__(SFH_MAP_LIST_REP SFH_PARENS (F, __VA_ARGS__))



// SFH_MAP_PEEL(F,(P),...)
//   maps macro 'F' across the vararg list where P
//   is one or more additional parameters (enclosed in parens)
//
// examples of passing one and two:
//   SFH_MAP_PEEL(F,(A),0,1,...)   → F(A,0) F(A,1) ...
//   SFH_MAP_PEEL(F,(A,B),0,1,...) → F(A,B,0) F(A,B,1) ...
//
// SFH_SMAP_PEEL performs same but for lists limited to 8 or less

#define SFH_MAP_PEEL_X(F,P,rescan,...) __VA_OPT__(rescan(SFH_MAP_PEEL_(F,P, __VA_ARGS__)))
#define SFH_MAP_PEEL(F,P,...) SFH_MAP_PEEL_X(F,P,SFH_EXPAND,__VA_ARGS__)
#define SFH_MAP_PEEL_REP() SFH_MAP_PEEL_

#define SFH_MAP_PEEL_(F,P,E, ...)                           \
  F(SFH_SIMPLE_FLATTEN(P),E)                                \
  __VA_OPT__(SFH_MAP_PEEL_REP SFH_PARENS (F,P,__VA_ARGS__))


// SFH_THROUGH((P),...)
//   expands the list of macros with parameter(s) P
//
//   SFH_THROUGH((P),A,B,...) → A(P) B(P) ...

#define SFH_THROUGH_X(macro,rescan,...) __VA_OPT__(rescan(SFH_THROUGH_(macro, __VA_ARGS__)))
#define SFH_THROUGH(macro,...) SFH_THROUGH_X(macro,SFH_EXPAND,__VA_ARGS__)
#define SFH_THROUGH_REP() SFH_THROUGH_

#define SFH_THROUGH_(params, macro, ...)  macro params __VA_OPT__(SFH_THROUGH_REP SFH_PARENS (params, __VA_ARGS__))

// SFH_THROUGH((P),...) but makes comma seperated list
#define SFH_THROUGH_LIST_X(F,rescan,...) SFH_DROP(__VA_OPT__(rescan(SFH_THROUGH_LIST_(F, __VA_ARGS__))))
#define SFH_THROUGH_LIST(F,...) SFH_THROUGH_LIST_X(F,SFH_EXPAND,__VA_ARGS__)
#define SFH_THROUGH_LIST_REP() SFH_THROUGH_LIST_

#define SFH_THROUGH_LIST_(P, macro, ...)  ,macro P __VA_OPT__(SFH_THROUGH_LIST_REP SFH_PARENS (P, __VA_ARGS__))


// SFH_REVERSE(...) : reverses the list
//   SFH_REVERSE(0,1,2,...,N) → N,...,2,1,0
#define SFH_REVERSE_X(rescan,...)  __VA_OPT__(rescan(SFH_REVERSE_(__VA_ARGS__)))
#define SFH_REVERSE(...) SFH_REVERSE_X(SFH_EXPAND,__VA_ARGS__)
#define SFH_REVERSE_REP() SFH_REVERSE_

#define SFH_REVERSE_(x, ...) __VA_OPT__(SFH_REVERSE_REP SFH_PARENS (__VA_ARGS__),) x 


//════════════════════════════════════════════════════════════════════════════════════
// argument count based macros. The pro of being limited and wordyier is that
// they require the preprocessor to perform a very small fixed number of expansions
// steps per macro invocation. The common structure here takes 7.

// for argcount based selection expansions
#define SFH_ARG_COUNT_MAX 8
#define SFH_ARGCOUNT_(_1,_2,_3,_4,_5,_6,_7,_8,_E,...) _E
#define SFH_ARGCOUNT(...) SFH_ARGCOUNT_(__VA_OPT__(__VA_ARGS__,) 8,7,6,5,4,3,2,1,0)

#define SFH_CAT0()
#define SFH_CAT1(A) A
#define SFH_CAT2(A,B) A##B
#define SFH_CAT3(A,B,C) A##B##C
#define SFH_CAT4(A,B,C,D) A##B##C##D
#define SFH_CAT5(A,B,C,D,E) A##B##C##D##E
#define SFH_CAT6(A,B,C,D,E,F) A##B##C##D##E##F
#define SFH_CAT7(A,B,C,D,E,F,G) A##B##C##D##E##F##G
#define SFH_CAT8(A,B,C,D,E,F,G,H) A##B##C##D##E##F##G##H

#define SFH_CAT(...) SFH_PART_8(__VA_ARGS__,SFH_CAT8,SFH_CAT7,SFH_CAT6,SFH_CAT5,SFH_CAT4,SFH_CAT3,SFH_CAT2,SFH_CAT1,SFH_CAT0)(__VA_ARGS__)

// require at least as long as the specified length
#define SFH_PART_0(x,...) x
#define SFH_PART_1(_0,x,...) x
#define SFH_PART_2(_0,_1,x,...) x
#define SFH_PART_3(_0,_1,_2,x,...) x
#define SFH_PART_4(_0,_1,_2,_3,x,...) x
#define SFH_PART_5(_0,_1,_2,_3,_4,x,...) x
#define SFH_PART_6(_0,_1,_2,_3,_4,_5,x,...) x
#define SFH_PART_7(_0,_1,_2,_3,_4,_5,_6,x,...) x
#define SFH_PART_8(_0,_1,_2,_3,_4,_5,_6,_7,x,...) x

#define SFH_PART(N,...) __VA_OPT__(SFH_CAT2(SFH_PART_,N)(__VA_ARGS__))

// SEE: SFH_MAP(F,...)
#define SFH_SMAP_0(F)
#define SFH_SMAP_1(F,A)               F(A)
#define SFH_SMAP_2(F,A,B)             F(A) F(B)
#define SFH_SMAP_3(F,A,B,C)           F(A) F(B) F(C)
#define SFH_SMAP_4(F,A,B,C,D)         F(A) F(B) F(C) F(D)
#define SFH_SMAP_5(F,A,B,C,D,E)       F(A) F(B) F(C) F(D) F(E)
#define SFH_SMAP_6(F,A,B,C,D,E,G)     F(A) F(B) F(C) F(D) F(E) F(G)
#define SFH_SMAP_7(F,A,B,C,D,E,G,H)   F(A) F(B) F(C) F(D) F(E) F(G) F(H)
#define SFH_SMAP_8(F,A,B,C,D,E,G,H,I) F(A) F(B) F(C) F(D) F(E) F(G) F(H) F(I)

#define SFH_SMAP(F,...)    __VA_OPT__(SFH_SMAP_(F, SFH_ARGCOUNT(__VA_ARGS__), __VA_ARGS__))
#define SFH_SMAP_(F,N,...) SFH_CAT2(SFH_SMAP_,N)(F,__VA_ARGS__)

// SEE: SFH_MAP_LIST(F,...)
#define SFH_SMAP_LIST_1(F,A)               F(A)
#define SFH_SMAP_LIST_2(F,A,B)             F(A), F(B)
#define SFH_SMAP_LIST_3(F,A,B,C)           F(A), F(B), F(C)
#define SFH_SMAP_LIST_4(F,A,B,C,D)         F(A), F(B), F(C), F(D)
#define SFH_SMAP_LIST_5(F,A,B,C,D,E)       F(A), F(B), F(C), F(D), F(E)
#define SFH_SMAP_LIST_6(F,A,B,C,D,E,G)     F(A), F(B), F(C), F(D), F(E), F(G)
#define SFH_SMAP_LIST_7(F,A,B,C,D,E,G,H)   F(A), F(B), F(C), F(D), F(E), F(G), F(H)
#define SFH_SMAP_LIST_8(F,A,B,C,D,E,G,H,I) F(A), F(B), F(C), F(D), F(E), F(G), F(H), F(I)

#define SFH_SMAP_LIST(F,...)    __VA_OPT__(SFH_SMAP_LIST_(F, SFH_ARGCOUNT(__VA_ARGS__), __VA_ARGS__))
#define SFH_SMAP_LIST_(F,N,...) SFH_CAT2(SFH_SMAP_LIST_,N)(F,__VA_ARGS__)

// SEE: SFH_MAP_PEEL
#define SFH_SMAP_PEEL_1(F,X,A)               F(X,A)
#define SFH_SMAP_PEEL_2(F,X,A,B)             F(X,A) F(X,B)
#define SFH_SMAP_PEEL_3(F,X,A,B,C)           F(X,A) F(X,B) F(X,C)
#define SFH_SMAP_PEEL_4(F,X,A,B,C,D)         F(X,A) F(X,B) F(X,C) F(X,D)
#define SFH_SMAP_PEEL_5(F,X,A,B,C,D,E)       F(X,A) F(X,B) F(X,C) F(X,D) F(X,E)
#define SFH_SMAP_PEEL_6(F,X,A,B,C,D,E,G)     F(X,A) F(X,B) F(X,C) F(X,D) F(X,E) F(X,G)
#define SFH_SMAP_PEEL_7(F,X,A,B,C,D,E,G,H)   F(X,A) F(X,B) F(X,C) F(X,D) F(X,E) F(X,G) F(X,H)
#define SFH_SMAP_PEEL_8(F,X,A,B,C,D,E,G,H,I) F(X,A) F(X,B) F(X,C) F(X,D) F(X,E) F(X,G) F(X,H) F(X,I)

#define SFH_SMAP_PEEL(F,X,...)    __VA_OPT__(SFH_SMAP_PEEL_(F,X, SFH_ARGCOUNT(__VA_ARGS__), __VA_ARGS__))
#define SFH_SMAP_PEEL_(F,X,N,...) SFH_CAT2(SFH_SMAP_PEEL_,N)(F,SFH_SIMPLE_FLATTEN(X),__VA_ARGS__)

// SEE: SFH_THROUGH
#define SFH_STHROUGH_1(P,A)               A P 
#define SFH_STHROUGH_2(P,A,B)             A P  B P 
#define SFH_STHROUGH_3(P,A,B,C)           A P  B P  C P 
#define SFH_STHROUGH_4(P,A,B,C,D)         A P  B P  C P  D P 
#define SFH_STHROUGH_5(P,A,B,C,D,E)       A P  B P  C P  D P  E P 
#define SFH_STHROUGH_6(P,A,B,C,D,E,F)     A P  B P  C P  D P  E P  F P 
#define SFH_STHROUGH_7(P,A,B,C,D,E,F,G)   A P  B P  C P  D P  E P  F P  G P 
#define SFH_STHROUGH_8(P,A,B,C,D,E,F,G,H) A P  B P  C P  D P  E P  F P  G P  H P 

#define SFH_STHROUGH(F,...)    __VA_OPT__(SFH_STHROUGH_(F, SFH_ARGCOUNT(__VA_ARGS__), __VA_ARGS__))
#define SFH_STHROUGH_(F,N,...) SFH_CAT2(SFH_STHROUGH_,N)(F,__VA_ARGS__)

// SEE: SFH_THROUGH_LIST
#define SFH_STHROUGH_LIST_1(P,A)               A P 
#define SFH_STHROUGH_LIST_2(P,A,B)             A P,  B P 
#define SFH_STHROUGH_LIST_3(P,A,B,C)           A P,  B P,  C P 
#define SFH_STHROUGH_LIST_4(P,A,B,C,D)         A P,  B P,  C P,  D P 
#define SFH_STHROUGH_LIST_5(P,A,B,C,D,E)       A P,  B P,  C P,  D P,  E P 
#define SFH_STHROUGH_LIST_6(P,A,B,C,D,E,F)     A P,  B P,  C P,  D P,  E P,  F P 
#define SFH_STHROUGH_LIST_7(P,A,B,C,D,E,F,G)   A P,  B P,  C P,  D P,  E P,  F P,  G P 
#define SFH_STHROUGH_LIST_8(P,A,B,C,D,E,F,G,H) A P,  B P,  C P,  D P,  E P,  F P,  G P,  H P 

#define SFH_STHROUGH_LIST(F,...)    __VA_OPT__(SFH_STHROUGH_LIST_(F, SFH_ARGCOUNT(__VA_ARGS__), __VA_ARGS__))
#define SFH_STHROUGH_LIST_(F,N,...) SFH_CAT2(SFH_STHROUGH_LIST_,N)(F,__VA_ARGS__)

// returns the list with the last element removed
#define SFH_SMOST_1(A)
#define SFH_SMOST_2(A,B)             A
#define SFH_SMOST_3(A,B,C)           A,B
#define SFH_SMOST_4(A,B,C,D)         A,B,C
#define SFH_SMOST_5(A,B,C,D,E)       A,B,C,D
#define SFH_SMOST_6(A,B,C,D,E,F)     A,B,C,D,E
#define SFH_SMOST_7(A,B,C,D,E,F,G)   A,B,C,D,E,F
#define SFH_SMOST_8(A,B,C,D,E,F,G,H) A,B,C,D,E,F,G

#define SFH_SMOST(...) __VA_OPT__(SFH_SMOST_(SFH_ARGCOUNT(__VA_ARGS__), __VA_ARGS__))
#define SFH_SMOST_(N,...) SFH_CAT2(SFH_SMOST_,N)(__VA_ARGS__)



//────────────────────────────────────────────────────────────────────────────────────
// "function" overloading via a single _Generic. Extends Simon Tatham's example (1)
// for 2-ary functions to n-ary. The macro creates a parameter type signature
// then match the individual signtures to direct to desired target.
//
// 1) https://www.chiark.greenend.org.uk/~sgtatham/quasiblog/c11-generic/
//    SEE: type_tuple_switch(x, y)
//
// strawman that matches 1 & 2 parameter atan functions
//  #define atan(...) _Generic(SFH_GENERIC_SIG(__VA_ARGS__), \
//    void (*)(float)          : atanf,                      \
//    void (*)(double)         : atan,                       \
//    void (*)(float, float)   : atan2f,                     \
//    void (*)(double, double) : atan2)(__VA_ARGS__)

#define SFH_GENERIC_SIG(...) (void(*)(SFH_SMAP_LIST(typeof,__VA_ARGS__)))NULL


//────────────────────────────────────────────────────────────────────────────────────
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

#define SFH_DEF_PARAM_2(F,D2,...)       SFH_PART_2(__VA_ARGS__, F(__VA_ARGS__), F(__VA_ARGS__,D2),)
#define SFH_DEF_PARAM_3(F,D2,D3,...)    SFH_PART_3(__VA_ARGS__, F(__VA_ARGS__), F(__VA_ARGS__,D2), F(__VA_ARGS__,D2,D3),)
#define SFH_DEF_PARAM_4(F,D2,D3,D4,...) SFH_PART_4(__VA_ARGS__, F(__VA_ARGS__), F(__VA_ARGS__,D2), F(__VA_ARGS__,D2,D3), F(__VA_ARGS__,D2,D3,D4),)


// map the supplied # of parameters to a unique macro or C call (up to 8)
//   #define foo(...) SFH_PARAM_MAP(foo,__VA_ARGS__)
//
//   foo()      → foo_0()
//   foo(a)     → foo_1(a)
//   foo(a,b)   → foo_2(a,b)
//   foo(a,b,c) → foo_3(a,b,c)

#define SFH_PARAM_MAP(F,...)    SFH_PARAM_MAP_(F, SFH_ARGCOUNT(__VA_ARGS__), __VA_ARGS__)
#define SFH_PARAM_MAP_(F,N,...) SFH_CAT2(F##_,N)(__VA_ARGS__)

