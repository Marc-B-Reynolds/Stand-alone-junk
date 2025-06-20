// -*- coding: utf-8 -*-
// Marc B. Reynolds, 2002-2025
// Public Domain under http://unlicense.org, see link for details.

// Terminal output oriented (progressive) report tables:
// • progressive: each line is written as submitted. The origin
//   is statistical testing were I want to see progress as it's
//   being made.
// • immediate mode: no internal state modification (after inital
//   set-up and initialization)
// • the "design" is focused on being able to toss one together
//   with minimal fuss. A sad (minor) hic-up is since it uses
//   varargs it's limited by vararg promotion rules: floats are
//   promoted to doubles (handy), integers smaller than 32-bits
//   are promoted to 32, 64-bit and pointers.
//     1) 32-bit integer
//     2) 64-bit integer
//     3) doubles
//     4) pointers (strings, user provided formatting, etc)
// • no effort is made to be internally efficient (speed or space)
// • information trumps formatting: (expand)
// • all text is assumed to be UTF8 (which includes 7-bit ASCII)
//   (note when SGR codes are OK. But directly supplied aren't stripped when
//    `plain` is set)
//   • Visual C needs the option `/utf-8` set.
//   • Under windows probably need to call `SetConsoleOutputCP(CP_UTF8)`

#pragma once

#define SFH_REPORT_TABLE   // marker define

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>

#define REPORT_TABLE_MAJOR    2
#define REPORT_TABLE_MINOR   12
#define REPORT_TABLE_REVISION 0

// maximum number of columns supported
#ifndef REPORT_TABLE_MAX_COL
#define REPORT_TABLE_MAX_COL 16
#endif

// min number of character in column data
#ifndef REPORT_TABLE_MIN_WIDTH
#define REPORT_TABLE_MIN_WIDTH 3
#endif

// max number of character in column data
#ifndef REPORT_TABLE_MAX_WIDTH
#define REPORT_TABLE_MAX_WIDTH 64
#endif


// output building buffer size: allow for a reasonable amount
// of multi-byte characters
#ifndef REPORT_TABLE_BUFFER_LEN
#define REPORT_TABLE_BUFFER_LEN (4*REPORT_TABLE_MAX_WIDTH)
#endif

static_assert(REPORT_TABLE_MIN_WIDTH > 2,                        "humm");
static_assert(REPORT_TABLE_MAX_WIDTH < 255,                      "humm");
static_assert(REPORT_TABLE_MAX_WIDTH > REPORT_TABLE_MIN_WIDTH,   "humm");
static_assert(REPORT_TABLE_BUFFER_LEN >= REPORT_TABLE_MAX_WIDTH, "humm");


typedef struct report_table_s  report_table_t;
typedef struct report_tbuild_s report_tbuild_t;

// name the datum pulled from varargs. illegal is first so it will
// be the "default". The reasonable way to set up a table is using
// designated initializers (as per the local macros) where all
// unspecified values are implicitly initialized to zero.
//
// recall varargs promotes sizes smaller than 32-bit integers
// and doubles to each respectively.
typedef enum {
  report_ttype_illegal,  // { }
  report_ttype_ptr,      // pointer
  report_ttype_i32,      // 32 bit integer (smaller ints promoted to) 
  report_ttype_i64,      // 64 bit integer
  report_ttype_f64,      // floating point (smaller promoted to doubles)
  report_ttype_num_types
} report_ttype_t;


// varargs are gathered and dispatched. At the dispatch stage we
// have 3 types: 64-bit integers, 64-bit floats and pointers.
typedef void (report_table_iformat_t)(report_tbuild_t*, uint64_t);
typedef void (report_table_fformat_t)(report_tbuild_t*, double);
typedef void (report_table_pformat_t)(report_tbuild_t*, void*);

typedef enum {
  report_table_justify_auto,    // marker that user hasn't specified (must be first)
  report_table_justify_right,   //   as it defaults to right
  report_table_justify_center,  
  report_table_justify_left,  
} report_table_justify_t;


// builtin framing sytles. At table intialization the enum is
// used to set a pointer to a `report_table_style_def_t` with
// is actually when spewing out text.
// There's hardwired bits than examine the stored enum for
// markdown to special case some formatting.
typedef enum {
  report_table_style_1990,
  report_table_style_ascii,
  report_table_style_markdown
} report_table_style_t;

// data on how to display each cell
typedef struct {
  uint8_t width;                // total character width (user provided)

  uint8_t w;                    // filtered width (this is used for rendering)
  uint8_t nl;                   // number of displayed characteres in 'name'
  
  const char* name;             // header text (must be UTF-8)
  const char* fmt;              // formatting string (formatter specific, fprintf style 4 std)
  const char* sgr;              // any SGR formatting for the cell (column plain disables)

  report_ttype_t    type;       // how to grab a legal datum from varargs
  report_table_justify_t just;  // default to right

  // below here is formatting function specific data
  uint8_t  digits;              // 
  uint8_t  prec;                // 
  uint8_t  extra;               // 
  uint32_t compact:    1;       // 
  uint32_t truncate:   1;       // 
  uint32_t is_32:      1;       // 
  uint32_t is_signed:  1;       // also used by the dispatcher to promote 32 to 64 bit ints

  // type specific formatter. integer and float use baked-in formating
  // if `null`. can be runtime flipped by user.
  union {
    report_table_iformat_t* iformat;  // integer
    report_table_fformat_t* fformat;  // float
    report_table_pformat_t* pformat;  // pointer
  };

  // any extra data needed for {i,p,f}format
  const void* fdata;
  
} report_table_col_t;


// (potentially) multibyte character
// frame elements
typedef struct {
  char* r;                     // right
  char* i;                     // interior
  char* d;                     // divider
  char* l;                     // left
} report_table_style_set_t;

typedef struct {
  report_table_style_set_t t;  // header top
  report_table_style_set_t e;  // header end
  report_table_style_set_t i;  // between rows
  report_table_style_set_t b;  // bottom
  char* div;                   // row divider
} report_table_style_def_t;


typedef struct report_table_s {
  report_table_col_t col[REPORT_TABLE_MAX_COL];

  report_table_style_def_t* s; // frame drawing data

  uint8_t  num_col;            // number of columns in table (init time computed)
  uint8_t  indent;             // left space padding of table output

  char*    sgr;                // null or SGR start code for header text (ignored if plain set)
  char*    eol;                // string printed at end of line. defaults to "\n" can be
                               // set to anything (say end of row comment)
  
  uint32_t is_initializied:1;  // internal flag to mark `report_table_init` has been called
  uint32_t compact:1;          // request formatter to make output smaller ()
  uint32_t plain:1;            // request xxx

  report_table_style_t style;  // frame enum. used at init time to set `s` above and
                               // runtime marker for some markdown specific formatting

  void* user_data;             // you tell me
} report_table_t;

#if 0
// mostly dead. should die.
typedef struct {
  char*   fmt;                 // format string
  void*   ffunc;               // formatting function
  uint8_t digits;              // 
  uint8_t prec;                //
  uint8_t extra;               //
  uint8_t pad;
} report_ttype_defaults_t;
#endif

// temp variables for row building
struct report_tbuild_s {
  report_table_col_t* col;     // current column being processed
  report_table_t*     table;   //   what table it is
  int                 i;       //   column index
  int                 bi;      // number of bytes written to buffer (progressive only)
  FILE*               file;    // file we're going to write to
  char                buffer[REPORT_TABLE_BUFFER_LEN];
};

// data for array formatting:
// first element uses: fmt, digits, prec from col
// non-zero defs in this structure replace for
// the remaining elements.
typedef struct {
  int n;                       // length of the array
  const char* eformat;         // how to format element
  const char* prefix;          // start string
  const char* suffix;          // end string
  const char* sep;             // seperator char
  uint8_t     digits;
  uint8_t     prec;
} report_table_aformat_t;

extern void report_table_u8_array (report_tbuild_t* b, void* data);
extern void report_table_u64_array(report_tbuild_t* b, void* data);
extern void report_table_f64_array(report_tbuild_t* b, void* data);

// built-in framing defs
extern report_table_style_def_t report_table_style_def_1990;
extern report_table_style_def_t report_table_style_def_ascii;
extern report_table_style_def_t report_table_style_def_markdown;

//***********************************************************************

// fill-in info that needs to be computed. called automatically from
// 
extern void report_table_init(report_table_t* table);

// base user functions
extern void report_table_header(FILE* file, report_table_t* table);
extern void report_table_row(FILE* file, report_table_t* table, ...);
extern void report_table_end(FILE* file, report_table_t* table);

// identical to report_table_row except via va_list
extern void report_table_row_va(FILE* file, report_table_t* table, va_list args);


//***********************************************************************
// For building custom formatting of cell data

// formats the current cell (in one shot)
extern void report_table_format(report_tbuild_t* b, const char* fmt, ...);

// incremental formating: append data to the buffer and returns the number of
// 'bytes' (not characters) added.
extern int report_table_format_inc(report_tbuild_t* b, const char* fmt, ...);

// submits the final buffer built using the previous
extern void report_table_format_done(report_tbuild_t* b);


//***********************************************************************
// expand macro that accepts [0,4] parameters (argcount method)
//   #define BAR(...)  REPORT_TABLE_DEF_X(BAR __VA_OPT__(,__VA_ARGS__))
//
// reduced expansion example (if limited to 2 instead of 4):
//   BAR()    → BAR_0()
//   BAR(X)   → BAR_1(X)
//   BAR(X,Y) → BAR_2(X,Y)
//
//   BAR(X,Y,Z,...) → Z(X,Y,Z,...) { illegal cases expansions }
//
// for every legal number of arguments the approp _n macro needs to be defined
// so if BAR accepts 0 or 2 then:
//   BAR_0 & BAR_2

#define REPORT_TABLE_DEF_X_(_0,_1,_2,_3,M,...) M
#define REPORT_TABLE_DEF_X(F,...) REPORT_TABLE_DEF_X_(__VA_OPT__(__VA_ARGS__,) F##_4,F##_3,F##_2,F##_1,F##_0)(__VA_ARGS__)

#if defined(__GNUC__)
#define REPORT_TABLE_DEF_ERROR(msg) _Pragma("GCC error \"" msg  "\"")
#else
#define REPORT_TABLE_DEF_ERROR(msg) 
#endif

//***********************************************************************
// table define helper macros : integers using default formatting
//


// 32/64 bit signed/unsided integer helper macros helpers
//   N : column header text
//   F : format string
//   D : expected max digits 
//   S : 0=unsigned, 1=signed
// (extra is D+S-1: +S for sign, -1 to account for
//  digits=1)

#define REPORT_TABLE_INT(N,F,E)  \
  .name   = N,                   \
  .fmt    = F,                   \
  .extra  = E


// simple unsigned/signed integers.
// example unsigned 32-bit integers:
//   REPORT_TABLE_U32("name", digits)
//   REPORT_TABLE_U32("name")
// 
// and likewise for signed (S) and 64-bit. 
// 
// row header name "name" and "digits" is the
// expected max number of digits that will be
// displayed.

#define REPORT_TABLE_I64(N,F,D,E) \
  REPORT_TABLE_INT(N,F,D+E-1),    \
  .type=report_ttype_i64,         \
  .digits = 1                     \

#define REPORT_TABLE_I32(N,F,D,E) \
  REPORT_TABLE_INT(N,F,D+E-1),    \
  .type=report_ttype_i32,         \
  .digits = 1,                    \
  .is_32  = 1

// hexadecimal 32/64 bit integers
// (other formats set digits=1)
#define REPORT_TABLE_H_(N,F,D) \
  .name   = N,                 \
  .digits = D,                 \
  .fmt    = F


// make the expansions # parameter explict so perhaps easier to
// read/write/mod also makes unsupported number of parameters
// a compile time error.

#define REPORT_TABLE_H32_2(N,D) REPORT_TABLE_H_(N,"%0*lx",D), .type=report_ttype_i32, .is_32=1
#define REPORT_TABLE_H64_2(N,D) REPORT_TABLE_H_(N,"%0*lx",D), .type=report_ttype_i64

#define REPORT_TABLE_U32_2(N,D) REPORT_TABLE_I32(N,"%*lu", D,0)
#define REPORT_TABLE_U64_2(N,D) REPORT_TABLE_I64(N,"%*lu", D,0)

#define REPORT_TABLE_S32_2(N,D) REPORT_TABLE_I32(N,"% *ld",D,1), .is_signed=1
#define REPORT_TABLE_S64_2(N,D) REPORT_TABLE_I64(N,"% *ld",D,1), .is_signed=1

#define REPORT_TABLE_H32_1(N)   REPORT_TABLE_H32_2(N,8)   
#define REPORT_TABLE_H64_1(N)   REPORT_TABLE_H64_2(N,16)   
#define REPORT_TABLE_U32_1(N)   REPORT_TABLE_U32_2(N,1)   
#define REPORT_TABLE_U64_1(N)   REPORT_TABLE_U64_2(N,1)   
#define REPORT_TABLE_S32_1(N)   REPORT_TABLE_S32_2(N,2)   
#define REPORT_TABLE_S64_1(N)   REPORT_TABLE_S64_2(N,2)   

// actual macros to call
#define REPORT_TABLE_H32(...) REPORT_TABLE_DEF_X(REPORT_TABLE_H32 __VA_OPT__(,__VA_ARGS__))
#define REPORT_TABLE_U32(...) REPORT_TABLE_DEF_X(REPORT_TABLE_U32 __VA_OPT__(,__VA_ARGS__))
#define REPORT_TABLE_S32(...) REPORT_TABLE_DEF_X(REPORT_TABLE_S32 __VA_OPT__(,__VA_ARGS__))

#define REPORT_TABLE_H64(...) REPORT_TABLE_DEF_X(REPORT_TABLE_H64 __VA_OPT__(,__VA_ARGS__))
#define REPORT_TABLE_U64(...) REPORT_TABLE_DEF_X(REPORT_TABLE_U64 __VA_OPT__(,__VA_ARGS__))
#define REPORT_TABLE_S64(...) REPORT_TABLE_DEF_X(REPORT_TABLE_S64 __VA_OPT__(,__VA_ARGS__))

//***********************************************************************
// table define helper macros : floating point

// helper macro helper (if you follow me)
#define REPORT_TABLE_F_(N,F,D,P)   \
  .name   = N,                     \
  .type   = report_ttype_f64,      \
  .digits = D,                     \
  .prec   = P,                     \
  .fmt    = F                      \

// %f style
// N : name of column
// I : max expected integer digits
// F : number of fractional digits
//  since digits is one: set extra to
//  "I+F+2"
#define REPORT_TABLE_F(N,I,F)      \
  REPORT_TABLE_F_(N,"% *.*f",1,F), \
  .extra = ((I)+(F)+2)

#define REPORT_TABLE_POS_F(N,I,F)  \
  REPORT_TABLE_F_(N,"%*.*f",1,F), \
  .extra = ((I)+(F)+1)


#define REPORT_TABLE_F32(N)  REPORT_TABLE_F(N,4, 8) ,.is_32=1
#define REPORT_TABLE_F64(N)  REPORT_TABLE_F(N,4,16)

// %g style
#define REPORT_TABLE_G(N,D)          \
  REPORT_TABLE_F_(N,"% #-*.*g",D,D), \
  .extra = 6

// %e style
#define REPORT_TABLE_E(N,D)          \
  REPORT_TABLE_F_(N,"% #-*.*e",D,D), \
  .extra = 7


// decimal formats:
//  • log2(2^24) ≈  7.225
//  • log2(2^53) ≈ 15.955
#define REPORT_TABLE_G32(N)  REPORT_TABLE_G(N, 8) ,.is_32=1
#define REPORT_TABLE_G64(N)  REPORT_TABLE_G(N,16)
#define REPORT_TABLE_E32(N)  REPORT_TABLE_E(N, 7) ,.is_32=1
#define REPORT_TABLE_E64(N)  REPORT_TABLE_E(N,15)


// %a (hexdecimal formatting)
//  `D` number of fractional hex digits
//  • sign padding and left justified by default so the
//    decimal points align.
//  • extra = { ,-}, 0x{0,1}., p, {+,-}, and 2 digits
//    for the exponent for a total of 9 characters.

#define REPORT_TABLE_A(NAME,D)         \
  REPORT_TABLE_F_(NAME,"% -*.*a",D,D), \
  .just  = report_table_justify_left,  \
  .extra = 9

// default digits are to show the exact value of binary32
// and binary64. The number of fractional hex digits is
// explictly stored bits divided by 4:
//  • binary32: 23/4 =  5.75 so 6
//  • binary64: 52/4 = 13
#define REPORT_TABLE_A32(NAME) REPORT_TABLE_A(NAME, 6) ,.is_32=1
#define REPORT_TABLE_A64(NAME) REPORT_TABLE_A(NAME,13)



//***********************************************************************
// helper macros : pointers (including strings)

// standard string handler:
//   REPORT_TABLE_STR(name)
//   REPORT_TABLE_STR(name,max_characters)

#define REPORT_TABLE_STR_2(NAME,E) \
  .name    = NAME,                 \
  .type    = report_ttype_ptr,     \
  .pformat = report_table_str,     \
  .extra   = E

#define REPORT_TABLE_STR_1(NAME) REPORT_TABLE_STR_2(NAME,0) 

#define REPORT_TABLE_STR(...) REPORT_TABLE_DEF_X(REPORT_TABLE_STR __VA_OPT__(,__VA_ARGS__))
#define REPORT_TABLE_STRING   REPORT_TABLE_STR  // just an alias

extern report_table_pformat_t report_table_str;

// RGB foo
#define REPORT_TABLE_RGB_BLOCK(NAME,...)  \
  .name    = NAME,                        \
  .type    = report_ttype_ptr,            \
  .pformat = report_table_rgb_block,      \
  .just    = report_table_justify_center  \
   __VA_OPT__(,.prec=__VA_ARGS__)

extern report_table_pformat_t report_table_rgb_block;


//***********************************************************************
// full SGR codes

// aside: why don't all compilers support `\e`?
#define SGR_RESET      "\033[0m"
#define SGR_BOLD       "\033[1m"
#define SGR_FAINT      "\033[2m"
#define SGR_ITALIC     "\033[3m"
#define SGR_UNDERLINE  "\033[4m"
#define SGR_BLINK      "\033[5m"
#define SGR_INVERSE    "\033[7m"
#define SGR_FG         "\033[38m"
#define SGR_BG         "\033[48m"
#define SGR_OVERLINE   "\033[53m"

// need to be decimal constants
#define SGR_RGB(R,G,B)    "\033[38;2;" #R ";" #G ";" #B "m"
#define SGR_BG_RGB(R,G,B) "\033[48;2;" #R ";" #G ";" #B "m"
#define SGR_BOTH_RGB(FR,FG,FB, BR,BG,BB) "\033[38;2;" #FR ";" #FG ";" #FB ";48;2;" #BR ";" #BG ";" #BB "m"


//***********************************************************************

#if defined(REPORT_TABLE_IMPLEMENTATION)

#define report_tselect(A,B) (A) ? (A) : (B)


#if 0
// default format strings must be ASCII
const report_ttype_defaults_t
report_ttype_default[report_ttype_num_types] =
{
  [report_ttype_i32] = {.fmt="0x%0*X",  .ffunc = 0 },
  [report_ttype_i64] = {.fmt="0x%0*X",  .digits=1 },
  [report_ttype_f64] = {.fmt="% -*.*f", .digits=1, .prec=6},
  [report_ttype_ptr] = {.fmt="%p"},
};
#endif

// terminal dump styles: each must render to one character
report_table_style_def_t report_table_style_def_1990 =
  {
    .t = {.r="┌", .i="─", .d="┬", .l="┐"},
    .e = {.r="├", .i="─", .d="┼", .l="┤"},
    .i = {.r="├", .i="─", .d="┼", .l="┤"},
    .b = {.r="└", .i="─", .d="┴", .l="┘"},
    .div = "│"
  };

report_table_style_def_t report_table_style_def_ascii =
  {
    .t = {.r="+", .i="-", .d="+", .l="+"},
    .e = {.r="+", .i="-", .d="+", .l="+"},
    .i = {.r="+", .i="-", .d="+", .l="+"},
    .b = {.r="+", .i="-", .d="+", .l="+"},
    .div = "|"
  };

report_table_style_def_t report_table_style_def_markdown =
  {
    .t = {.r=" ", .i=" ", .d=" ", .l=" "},
    .e = {.r="|", .i="-", .d="|", .l="|"},
    .i = {.r="|", .i="-", .d="|", .l="|"},
    .b = {.r=" ", .i=" ", .d=" ", .l=" "},
    .div = "|"
  };


// map enum to pointer
report_table_style_def_t* report_table_style_map[] =
{
  [report_table_style_1990]     = &report_table_style_def_1990,
  [report_table_style_ascii]    = &report_table_style_def_ascii,
  [report_table_style_markdown] = &report_table_style_def_markdown,
};

static inline const char* report_fmt_select(const char* a, const char* b)
{
  if (a && a[0]) return a;
  if (b && b[0]) return b;
  return "";
}

// meh
int report_table_fprintf(FILE* file, report_table_t* table, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int len = vfprintf(file,fmt,args);
  va_end(args);

  (void)table;
  
  return len;
}


// super inefficient but who cares: used for any indentation
// and the divider lines between cells
static void report_table_char_rep(FILE* file, char* c, uint32_t n)
{
  while(n--) { fprintf(file,"%s",c); }
}

// naive UTF8 string length. just byte by byte
// counting non-continutation bytes (top two bits
// are 10)
static int report_table_utf8_len(const char* str)
{
  int len = 0;

  while (*str) {
    len += ((*str & 0xC0) != 0x80);
    str++;
  }
  return len;
}

// byte sequence can be UTF8 and SGR codes
// SGR isn't checked. Any byte sequence starting
// with escape (\e) until SGR command (m) is
// treated as if it's a legal (0 characters
// produced).
// This is only used for formating so if it's
// wrong (shrug). Fix the input.
static int report_table_utf8_sgr_len(char* str)
{
  int len = 0;

  while (*str) {
    char c = *str++;
    if (c != '\033') {
      len += ((c & 0xC0) != 0x80);
    }
    else {
      // scan through the SGR parameters
      while(*str != 'm') {
        if (*str != 0)
          str++;
        else
          return len;
      }
      // terminating 'm'
      str++;
    }
  }

  return len;
}

  
//***********************************************************************

void report_table_init(report_table_t* table)
{
  report_table_col_t* col = table->col;

  uint32_t n;
  
  // KISS: linear scan the column names to discover the number of columns
  for(n=0; (n < REPORT_TABLE_MAX_COL) && table->col[n].name; n++);

  table->num_col = (uint8_t)n;
  table->s       = report_table_style_map[table->style];

  if (table->eol == 0) table->eol = "\n";

  // walk the columns
  for (uint32_t i=0; i<n; i++) {
    const char* name = col->name;

    // initial simple defaults
    if (!col->fmt)  col->fmt = "";
    
    // figure out the width. this isn't designed to be "fancy"
    // but to allow the user to choose with a minimum(ish)
    // amount typing in the table def:

    // 1) check the length of the header text (expand if smaller than min)
    int hlen = report_table_utf8_len(name);
    col->nl = (uint8_t)hlen;

    hlen = (hlen > REPORT_TABLE_MIN_WIDTH) ? hlen : REPORT_TABLE_MIN_WIDTH;
    hlen = (hlen < REPORT_TABLE_MAX_WIDTH) ? hlen : REPORT_TABLE_MAX_WIDTH;
    
    // 2) max(request, hlen, (digits+extra))
    uint8_t width = col->width;
    uint8_t foo   = col->digits + col->extra;
    width = (width > hlen)   ? width : (uint8_t)hlen;
    width = (width > foo)    ? width : foo;
    
    // set internally used width
    col->w  = width;

    col++;
  }

  table->is_initializied = 1;
}


//***********************************************************************

// helper: performs any justification and does the output. doesn't perform
// any truncation
//   clen : character length
//   blen : byte length
static void report_table_out(report_tbuild_t* tb, const char* buffer, int clen, int blen)
{
  (void)blen; // unused ATM
  
  int width  = tb->col->w;
  int pr     = 0;
  int pl     = 0;
  
  if (clen < width) {
    int d = width-clen;
    switch(tb->col->just) {
      case report_table_justify_auto:
      case report_table_justify_right: pl=d; break;
      case report_table_justify_left:  pr=d; break;
      default:
        pr = d >> 1;
        pl = d - pr;
    }
  }

  report_table_fprintf(tb->file, tb->table, "%*s%s%*s",pl,"",buffer,pr,"");
}

static inline int report_table_vsnprintf(report_tbuild_t* b, const char* fmt, va_list args)
{
  return vsnprintf(b->buffer, sizeof(b->buffer),fmt,args);
}

static inline int report_table_snprintf(report_tbuild_t* b, const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int  blen = report_table_vsnprintf(b,fmt,args);
  va_end(args);

  return blen;
}

// single format can produce the cell data
void report_table_format(report_tbuild_t* b, const char* fmt, ...)
{
  // perform the format
  va_list args;
  va_start(args, fmt);
  int  blen    = report_table_vsnprintf(b,fmt,args);
  va_end(args);

  // attempt to determine the number of rendered characters.
  int clen = report_table_utf8_sgr_len(b->buffer);

  report_table_out(b,b->buffer,clen,blen);
}

// progressive cell formatting (vs. all at once with report_table_format)
// returns the number of "bytes" written.
int report_table_format_inc(report_tbuild_t* b, const char* fmt, ...)
{
  char*    buffer = b->buffer + b->bi;
  uint32_t bmax   = (uint32_t)(REPORT_TABLE_BUFFER_LEN - b->bi);

  va_list args;
  va_start(args, fmt);
  int  blen = vsnprintf(buffer,bmax,fmt,args);
  va_end(args);

  b->bi += blen;

  return blen;
}

// completes `report_table_format_inc building` of cell
void report_table_format_done(report_tbuild_t* b)
{
  // attempt to determine the number of rendered characters.
  int clen = report_table_utf8_sgr_len(b->buffer);
  
  report_table_out(b,b->buffer,clen,b->bi);
}

//***********************************************************************
// dispatch input to formatting function. 

static void report_ttype_i64_d(report_tbuild_t* b, uint64_t v)
{
  report_table_iformat_t* ffunc = b->col->iformat;  

  if (ffunc == 0) {
    // handle standard integer formatting
    const char* fmt = b->col->fmt;
    int digits = b->col->digits;
    int len    = report_table_snprintf(b, fmt, digits, v);
    
    report_table_out(b,b->buffer,len,len);
    return;
  }
  
  ffunc(b,v);
}

static void report_ttype_f64_d(report_tbuild_t* b, double v)
{
  report_table_fformat_t* ffunc = b->col->fformat;  

  if (ffunc == 0) {
    // handle standard floating point formatting
    report_table_col_t* c = b->col;

    report_table_format(b,c->fmt,c->digits,c->prec,v);
    return;
  }

  ffunc(b,v);
}

static void report_ttype_ptr_d(report_tbuild_t* b, void* data)
{
  report_table_pformat_t* ffunc = b->col->pformat;  

  if (ffunc) {
    ffunc(b,data);
    return;
  }

  // should had a warning that can be disabled
  report_table_format(b, "%s", "---");  
}


// 32-bit integers: promote and forward to 64-bit integer version
static void report_ttype_i32_d(report_tbuild_t* b, uint32_t v)
{
  uint64_t p = (uint64_t)v;

  if (b->col->is_signed == 1)
    p = (uint64_t)((int64_t)((int32_t)v));

  report_ttype_i64_d(b,p);
}

// an illegal type has been hit. we've a good shot at being 
// UB since a pointer has been pulled from varargs.
static void report_ttype_ill_d(report_tbuild_t* b, void* v)
{
  (void)v;                                   // old-skool unused

  // might as well be a dork about it
  report_table_format(b, "%s",
                      SGR_BLINK SGR_INVERSE "\033[6m"
                      "ILLEGAL"
                      SGR_RESET);

  // try to get this out in case we go "boom" soon.
  fflush(b->file);
}


//***********************************************************************

#if 0
// meh: some old compact snippets.

// optionally strip leading 0x from %a style
{
  report_table_col_t* c = b->col;

  int   len = report_table_snprintf(b, c->fmt, c->digits, c->prec, v);
  char* r   = b->buffer;

  // nuke the 0x prefix is requested
  if ((b->table->compact)|(b->col->compact)) {
    int i = 2;
    if (b->buffer[1] == '0') { b->buffer[2] = b->buffer[0]; }
    r   += i;
    len -= i;
  }

  //....
}

// optionally strip any leading 0 is exponent for %e style
{
  report_table_col_t* c = b->col;
  int   len = report_table_snprintf(b, c->fmt, c->digits, v);

  // shorten: nuke any leading zero exponent
  if ((b->table->compact)|(b->col->compact)) {
    if (b->buffer[len-2] == '0') {
      b->buffer[len-2] = b->buffer[len-1];
      b->buffer[len-1] = 0;
    }
  }

  report_table_out(b,b->buffer,len,len);
}
#endif

//***********************************************************************

// rename: UTF + SGR formating helper
static void report_table_str_i(report_tbuild_t* b, char* buffer, int blen)
{
  // clen = number of displayed characters. If
  // this is computed wrong then it just screws
  // up the formating.
  int clen = report_table_utf8_sgr_len(buffer);
  int w    = b->col->w;

  // does a bunch of extra work in the "hard" cases
  // when truncation is enabled (column's truncate is 1)
  // expected to be a tiny number over all & also
  // expect that the "reporting" code time isn't
  // statistically significant.
  
  // no truncation enabled or if we fit: great
  if (clen <= w || (b->col->truncate == 0)) {
    report_table_out(b,buffer,clen,clen);
    return;
  }
  
  // if the # of characters and bytes are equal: easy
  if (blen == clen) {
    buffer[w] = 0;
    report_table_out(b,buffer,clen,clen);
    return;
  }

  // TODO: add trunctation for the harder case
  report_table_out(b,buffer,clen,clen);
}

//***********************************************************************
//

// standard string parameter handler: UTF8 + SGR codes (but should use
// sgr variant i
void report_table_str(report_tbuild_t* b, void* data)
{
  int blen = report_table_snprintf(b,"%s", (char*)data);
  report_table_str_i(b,b->buffer,blen);
}


// string with an SGR code for the cell. 
void report_table_sgr_str(report_tbuild_t* b, void* data)
{
  char* s = data;
  int   len;
  
  // should be at leaf level so any background covers all of the column
  // (if it's a pointer format then we know to check)
  if (b->table->plain || (b->col->fmt==0))
    len = report_table_snprintf(b,"%s",s);
  else
    len = report_table_snprintf(b,"%s%s" "\033[0m", b->col->fmt,s);
  
  report_table_str_i(b,b->buffer,len);
}

//***********************************************************************
// directly dumping elements of an array is super limited (to be reasonable
// in a table cell) but it generically covers small tuples.

// sign/unsigned bytes (handled by eformat)
void report_table_u8_array(report_tbuild_t* b, void* data)
{
  const report_table_aformat_t* af = b->col->fdata;
  
  uint8_t*  a = (uint8_t*)data;
  int       d = report_tselect(b->col->digits, af->digits);
  int       n = af->n;

  const char* fmt = report_fmt_select(b->col->fmt, af->eformat);

  report_table_format_inc(b,"%s", af->prefix);
  report_table_format_inc(b, fmt,  d,a[0]);

  d = report_tselect(af->digits, d);
  
  for(int i=1; i<n; i++) {
    report_table_format_inc(b,"%s", af->sep);
    report_table_format_inc(b, af->eformat, d,a[i]);
  }

  report_table_format_inc(b,"%s",af->suffix);
  report_table_format_done(b);
}

// sign/unsigned 64-bit ints (handled by eformat)
void report_table_u64_array(report_tbuild_t* b, void* data)
{
  const report_table_aformat_t* af = b->col->fdata;
  
  uint64_t* a = (uint64_t*)data;
  int       d = report_tselect(b->col->digits, af->digits);
  int       n = af->n;

  const char* fmt = report_fmt_select(b->col->fmt, af->eformat);

  report_table_format_inc(b,"%s", af->prefix);
  report_table_format_inc(b, fmt, d,a[0]);

  d = report_tselect(af->digits, d);

  for(int i=1; i<n; i++) {
    report_table_format_inc(b,"%s", af->sep);
    report_table_format_inc(b, fmt, d,a[i]);
  }

  report_table_format_inc(b,"%s",af->suffix);
  report_table_format_done(b);
}

// binary64
void report_table_f64_array(report_tbuild_t* b, void* data)
{
  const report_table_aformat_t* af = b->col->fdata;
  
  double* a = (double*)data;
  int     d = report_tselect(b->col->digits, af->digits);
  int     p = report_tselect(b->col->prec,   af->prec);
  int     n = af->n;

  const char* fmt = report_fmt_select(b->col->fmt, af->eformat);

   report_table_format_inc(b,"%s", af->prefix);
  report_table_format_inc(b, fmt,  d,p,a[0]);

  d = report_tselect(af->digits, d);
  p = report_tselect(af->prec,   p);
  
  for(int i=1; i<n; i++) {
    report_table_format_inc(b,"%s", af->sep);
    report_table_format_inc(b, af->eformat, d,p,a[i]);
  }

  report_table_format_inc(b,"%s",af->suffix);
  report_table_format_done(b);
}


// 3 bytes: {r,g,b} → RGB(r,g,b) {displayed in 3 digit decimal}
report_table_aformat_t report_table_text_rgb_format =
{
  .n       = 3,
  .eformat = "%*d",
  .prefix  = "RGB(",
  .sep     = ",",
  .suffix  = ")"
};

#define REPORT_TABLE_RGB_TEXT(N)           \
  .name    = "value", .width=16,.digits=3, \
  .type    = report_ttype_ptr,             \
  .pformat = report_table_u8_array,        \
  .fdata   = &report_table_text_rgb_format


//***********************************************************************

// outputs '.prec' spaces with foreground color RGB color data pointed: (3 bytes)
// assuming 'plain' isn't set: here it prints #RRGGBB {hexadecimal}
void report_table_rgb_block(report_tbuild_t* b, void* data)
{
  report_table_t* table = b->table;
  uint32_t n = b->col->prec ? b->col->prec : b->col->w;

  uint8_t* d = (uint8_t*)data;

  if (!table->plain)
    report_table_format(b,SGR_BG_RGB(%d,%d,%d) "%*s" "\033[0m",d[0],d[1],d[2],n,"");
  else
    report_table_format(b,"#%02X%02X%02X",d[0],d[1],d[2]);
}


//***********************************************************************

// hacky shorthand macros: should clean up at some point
#define report_table_ln(file,table)  report_table_fprintf(file,table,table->eol)
#define report_table_sep(file,table) report_table_fprintf(file,table,"%s",style->div)

void report_table_row_va(FILE* file, report_table_t* table, va_list args)
{
  if (!table->is_initializied) report_table_init(table);
  
  report_table_style_def_t* style = table->s;
  report_table_col_t*       col   = table->col;
  report_tbuild_t           tb;
  
  // set-up builder here. not using designated initialiation since
  // it's pointless to zero out the buffer. 
  tb.file      = file;
  tb.table     = table;
  tb.buffer[0] = 0;
  tb.bi        = 0;

  int do_sgr = table->plain == 0;
  
  // perform any indentation and draw left frame
  report_table_char_rep(file, " ", table->indent);
  report_table_sep(file,table);

  // walk all the column data
  for(int i=0; i<table->num_col; i++) {
    // update builder entries
    tb.i   = i;
    tb.col = table->col+i;

    if (do_sgr && (col->sgr != 0)) fprintf(file,"%s",col->sgr);

    // grab the parameter from varargs and dispatch
    switch(col[i].type) {
      case report_ttype_i32: report_ttype_i32_d(&tb, va_arg(args,uint32_t)); break;
      case report_ttype_i64: report_ttype_i64_d(&tb, va_arg(args,uint64_t)); break;
      case report_ttype_f64: report_ttype_f64_d(&tb, va_arg(args,double)  ); break;
      case report_ttype_ptr: report_ttype_ptr_d(&tb, va_arg(args,void*));    break;
      default:               report_ttype_ill_d(&tb, va_arg(args,void*));    break;
    }

    if (do_sgr && (col->sgr != 0)) fprintf(file,"%s",SGR_RESET);
    report_table_sep(file,table);
  }
  
  report_table_ln(file,table);
}


void report_table_row(FILE* file, report_table_t* table, ...)
{
  va_list args;
  va_start(args,table);
  report_table_row_va(file,table,args);
  va_end(args);
}


//***********************************************************************

// top of header, bottom of header and bottom of table divider lines
void report_table_div(FILE* file, report_table_t* table, report_table_style_set_t* set)
{
  if (!table->is_initializied) report_table_init(table);

  report_table_char_rep(file, " ", table->indent);
  fprintf(file, "%s", set->r);
  
  report_table_char_rep(file, set->i, table->col[0].w);
  
  for(uint32_t c=1; c<table->num_col; c++) {
    fprintf(file, "%s", set->d);
    report_table_char_rep(file, set->i,table->col[c].w);
  }
  
  fprintf(file, "%s", set->l);
}


// prints the table header
void report_table_header(FILE* file, report_table_t* table)
{
  if (!table->is_initializied) report_table_init(table);

  report_table_style_def_t* style = table->s;

  // top of header (skipped in enum is set to markdown)
  if (table->style != report_table_style_markdown) {
    report_table_div(file, table, &style->t);
    report_table_ln(file,table);
  }

  int do_sgr = (table->sgr != 0) && (table->plain == 0);

  report_table_char_rep(file, " ", table->indent);

  // each of the header entries
  for(uint32_t i=0; i<table->num_col; i++) {
    const char*    name = table->col[i].name;
    uint32_t hlen = table->col[i].nl;
    uint32_t w    = table->col[i].w;
    uint32_t d    = w-hlen;
    uint32_t pr   = d>>1;
    uint32_t pl   = d-pr;

    report_table_sep(file,table);

    // print the column header (with SGR code if provided and plain not set)
    // IMPORTANT: the odd construction is because fprintf doesn't understand
    // UTF-8 so we don't specify a width for any UTF-8 string and the padding
    // is performed with empty strings.
    if (!do_sgr)
      fprintf(file,
              "%*s"            // left pad format
              "%s"             // string (may be UTF-8)
              "%*s",           // right pad format
              pl,"",           // left pad data
              name,            // actual string
              pr,"");          // right pad
    
    else 
      fprintf(file,            // like above plus:
              "%s%"            // SGR code format
              "*s" 
              "%s"  
              "%*s" 
              "\033[0m",       // SGR reset
              table->sgr,      // SGR code
              pl,"",
              name, 
              pr,"");
  }

  report_table_sep(file,table);
  report_table_ln(file,table);

  // bottom of header (not handling markdown justification atm)
  report_table_div(file, table, &style->e);
  report_table_ln(file,table);
}

// prints the bottom border of the table
void report_table_end(FILE* file, report_table_t* table)
{
  if (!table->is_initializied) report_table_init(table);
  
  report_table_style_def_t* style = table->s;

  // skip if the style "enum" is markdown
  if (table->style != report_table_style_markdown) {
    report_table_div(file, table, &style->b);
    report_table_ln(file,table);
  }
}


#endif

