// Marc B. Reynolds, 2022-2024
// Public Domain under http://unlicense.org, see link for details.

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "bmat_i.h"

// MY EYES! YOUR EYES! 
// all junk. make workers and direct or better yet
// put my mini report library in SFH and add matrix support


//*******************************************************************
// output to PL & DSL dumps

// worker versions: pass in start of row string, element separator and end row string
void bmat_print_list_ni(uint64_t row, uint32_t b, char* rowS, char* elemS, char* rowE)
{
  printf("%s%c",rowS,(char)((row & 1)+'0'));
  row >>= 1;

  for(uint32_t i=1; i<b; i++) {
    printf("%s%c", elemS, (char)((row & 1)+'0'));
    row >>=1;
  }
  printf("%s", rowE);
}

void bmat_print_list_8i(bmat_param_8(m), char* rowS, char* elemS, char* rowE)
{
  static const uint32_t D = 8;
  
  uint8_t a[8];

  bmat_to_array_8(a,m);

  printf("%s", rowS);
  bmat_print_list_ni(a[0],D,rowS,elemS,rowE);
  
  for(uint32_t i=1; i<D; i++) {
    printf("%s", elemS);
    bmat_print_list_ni(a[i],D,rowS,elemS,rowE);
  }

  printf("%s\n",rowE);
}

void bmat_print_list_16i(bmat_param_16(m), char* rowS, char* elemS, char* rowE)
{
  static const uint32_t D = 16;
  
  uint16_t a[16];

  bmat_to_array_16(a,m);

  printf("%s", rowS);
  bmat_print_list_ni(a[0],D,rowS,elemS,rowE);
  
  for(uint32_t i=1; i<D; i++) {
    printf("%s", elemS);
    bmat_print_list_ni(a[i],D,rowS,elemS,rowE);
  }

  printf("%s\n",rowE);
}

void bmat_print_list_32i(bmat_param_32(m), char* rowS, char* elemS, char* rowE)
{
  static const uint32_t D = 32;
  
  uint32_t a[32];

  bmat_to_array_32(a,m);

  printf("%s", rowS);
  bmat_print_list_ni(a[0],D,rowS,elemS,rowE);
  
  for(uint32_t i=1; i<D; i++) {
    printf("%s", elemS);
    bmat_print_list_ni(a[i],D,rowS,elemS,rowE);
  }

  printf("%s\n",rowE);
}

void bmat_print_list_64i(bmat_param_64(m), char* rowS, char* elemS, char* rowE)
{
  static const uint32_t D = 64;
  
  uint64_t a[64];

  bmat_to_array_64(a,m);

  printf("%s", rowS);
  bmat_print_list_ni(a[0],D,rowS,elemS,rowE);
  
  for(uint32_t i=1; i<D; i++) {
    printf("%s", elemS);
    bmat_print_list_ni(a[i],D,rowS,elemS,rowE);
  }

  printf("%s\n",rowE);
}

void bmat_print_list_n(uint64_t row, uint32_t b) { bmat_print_list_ni(row,b,"{",",","}"); }

// curely braces and commas
void bmat_print_list_8 (bmat_param_8 (m)) { bmat_print_list_8i (m,"{",",","}"); }
void bmat_print_list_16(bmat_param_16(m)) { bmat_print_list_16i(m,"{",",","}"); }
void bmat_print_list_32(bmat_param_32(m)) { bmat_print_list_32i(m,"{",",","}"); }
void bmat_print_list_64(bmat_param_64(m)) { bmat_print_list_64i(m,"{",",","}"); }

// square brackets and commas
void bmat_print_mlist_8 (bmat_param_8 (m)) { bmat_print_list_8i (m,"[",",","]"); }
void bmat_print_mlist_16(bmat_param_16(m)) { bmat_print_list_16i(m,"[",",","]"); }
void bmat_print_mlist_32(bmat_param_32(m)) { bmat_print_list_32i(m,"[",",","]"); }
void bmat_print_mlist_64(bmat_param_64(m)) { bmat_print_list_64i(m,"[",",","]"); }

//*******************************************************************

void bmprt_row_start(char* prefix, uint32_t i) { printf("%s%2u │", prefix,i); }

void bmprt_row_end(uint32_t i, uint32_t w, char* marker)
{
  for(uint32_t n=0; n<i; n++) {} // disable warning - stuff to do
  for(uint32_t n=0; n<w; n++) {} // disable warning - stuff to do
  
  printf("%s\n", marker);
}

// hi-bit first - rename
void bvec_elems(uint64_t v, uint32_t b)
{
  uint64_t m = UINT64_C(1) << (b-1);
  for(uint32_t i=0; i<b; i++) {
    printf("%c", (v & m)!=0 ? '1':'.');
    m >>= 1;
  }
}

// low-bit first (for matrix rows) - rename
void bmprt_elems(uint64_t v, uint32_t b)
{
  uint64_t m = UINT64_C(1);
  for(uint32_t i=0; i<b; i++) {
    printf("%c", (v & m)!=0 ? '1':'.');
    m <<= 1;
  }
}

void bmprt_row_8 (uint64_t v) { bmprt_elems(v, 8); printf(" : %02lx │", v); }
void bmprt_row_16(uint64_t v) { bmprt_elems(v,16); printf(" : %04lx │", v); }
void bmprt_row_32(uint64_t v) { bmprt_elems(v,32); printf(" : %08lx │", v); }
void bmprt_row_64(uint64_t v) { bmprt_elems(v,64); printf(" : %016lx │", v); }

void bmprt_row_8_end (char* prefix, uint32_t i, char* marker) { printf("%s",prefix); bmprt_row_end(i, 8,marker); }
void bmprt_row_16_end(char* prefix, uint32_t i, char* marker) { printf("%s",prefix); bmprt_row_end(i,16,marker); }
void bmprt_row_32_end(char* prefix, uint32_t i, char* marker) { printf("%s",prefix); bmprt_row_end(i,32,marker); }
void bmprt_row_64_end(char* prefix, uint32_t i, char* marker) { printf("%s",prefix); bmprt_row_end(i,64,marker); }

void bmprt_int_array(uint32_t* a, uint32_t n)
{
  printf("{%u", a[0]);
  for(uint32_t i=1; i<n; i++)
    printf(",%u", a[i]);
  printf("}");
}

uint64_t bmprt_matrix_row(uint64_t m, uint32_t n)
{
  printf("{%lu", (m&1)); m >>= 1;
  
  for(uint32_t i=1; i<n; i++) { printf(",%lu", m&1); m >>=1; }
  
  printf("}");
  
  return m;
}

#if 0
void bmat_print_array_8(uint64_t* m, uint32_t n)
{
  printf("{");
  
  printf("}");
}
#endif

void bmat_print_8(char* prefix, bmat_param_8(m))
{
  uint8_t m0[8];

  bmat_to_array_8(m0,m);

  for(uint32_t i=0; i<8; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_8(m0[i]);
    bmprt_row_8_end("",1,"");
  }
}


void bmat_print2_8(char*   prefix,
		   bmat_param_8(a),
		   bmat_param_8(b))
{
  uint8_t m0[8];
  uint8_t m1[8];

  bmat_to_array_8(m0,a);
  bmat_to_array_8(m1,b);

  for(uint32_t i=0; i<8; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_8(m0[i]);
    bmprt_row_8(m1[i]);
    bmprt_row_8_end("",2,"");
  }
}

void bmat_print2_diff_8(char*   prefix,
			bmat_param_8(a),
			bmat_param_8(b))
{
  uint8_t m0[8];
  uint8_t m1[8];

  bmat_to_array_8(m0,a);
  bmat_to_array_8(m1,b);

  for(uint32_t i=0; i<8; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_8(m0[i]);
    bmprt_row_8(m1[i]);
    bmprt_row_8(m0[i]^m1[i]);
    bmprt_row_8_end(prefix,3,"");
  }
}


void bmat_print_16(char*    prefix,
		   bmat_param_16(a))
{
  uint16_t m0[16];

  bmat_to_array_16(m0,a);

  for(uint32_t i=0; i<16; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_16(m0[i]);
    bmprt_row_16_end(prefix,1,"");
  }
}

void bmat_print2_16(char*    prefix,
		    bmat_param_16(a),
		    bmat_param_16(b))
{
  uint16_t m0[16];
  uint16_t m1[16];

  bmat_to_array_16(m0,a);
  bmat_to_array_16(m1,b);

  for(uint32_t i=0; i<16; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_16(m0[i]);
    bmprt_row_16(m1[i]);
    bmprt_row_16_end(prefix,2,"");
  }
}

void bmat_print2_diff_16(char*    prefix,
			 bmat_param_16(a),
			 bmat_param_16(b))
{
  uint16_t m0[16];
  uint16_t m1[16];

  bmat_to_array_16(m0,a);
  bmat_to_array_16(m1,b);

  for(uint32_t i=0; i<16; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_16(m0[i]);
    bmprt_row_16(m1[i]);
    bmprt_row_16(m0[i]^m1[i]);
    bmprt_row_16_end(prefix,3,"");
  }
}

void bmat_print_32(char* prefix, bmat_param_32(a))
{
  uint32_t m[32];

  bmat_to_array_32(m,a);

  for(uint32_t i=0; i<32; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_32(m[i]);
    bmprt_row_32_end(prefix,1,"");
  }
}

void bmat_print2_32(char* prefix, bmat_param_32(a), bmat_param_32(b))
{
  uint32_t m0[32];
  uint32_t m1[32];

  bmat_to_array_32(m0,a);
  bmat_to_array_32(m1,b);

  for(uint32_t i=0; i<32; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_32(m0[i]);
    bmprt_row_32(m1[i]);
    bmprt_row_32_end(prefix,2,"");
  }
}

void bmat_print2_diff_32(char* prefix, bmat_param_32(a), bmat_param_32(b))
{
  uint32_t m0[32];
  uint32_t m1[32];

  bmat_to_array_32(m0,a);
  bmat_to_array_32(m1,b);

  for(uint32_t i=0; i<32; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_32(m0[i]);
    bmprt_row_32(m1[i]);
    bmprt_row_32(m0[i]^m1[0]);
    bmprt_row_32_end(prefix,3,"");
  }
}

void bmat_print_64(char* prefix, bmat_param_64(m))
{
  for(uint32_t i=0; i<64; i++) {
    bmprt_row_start(prefix,i);
    bmprt_row_64(m[i]);
    bmprt_row_64_end(prefix,1,"");
  }
}

#if defined(SWAR_AVX2_H)
//---------------------------------------------------------------------

// this is a dup..this file is such garbage
void bprint(uint64_t v, uint32_t b)
{
  uint64_t m = UINT64_C(1) << (b-1);
  for(uint32_t i=0; i<b; i++) {
    printf("%c", (v & m)!=0 ? '1':'.');
    m >>= 1;
  }
}

//----------------------------------------------------------------------

static char* print_vsep = "│";

#define wrap_reg_8(A,R)  uint8_t  A[32]; memcpy(A,&R,32);
#define wrap_reg_16(A,R) uint16_t A[16]; memcpy(A,&R,32);
#define wrap_reg_32(A,R) uint32_t A[ 8]; memcpy(A,&R,32);
#define wrap_reg_64(A,R) uint64_t A[ 4]; memcpy(A,&R,32);
#define back_2_reg(R,A)  memcpy(&R,A,32);

void dump_hex_8     (uint8_t  v)  { printf("%02x",v); }
void dump_hex_sep_8 (uint8_t  v)  { printf("%s%02x",print_vsep,v); }
void dump_hex_16    (uint16_t v)  { printf("%04x",v); }
void dump_hex_sep_16(uint16_t v)  { printf("%s%04x",print_vsep,v); }
void dump_hex_32    (uint32_t v)  { printf("%08x",v); }
void dump_hex_sep_32(uint32_t v)  { printf("%s%08x",print_vsep,v); }
void dump_hex_64    (uint64_t v)  { printf("%016lx",v); }
void dump_hex_sep_64(uint64_t v)  { printf("%s%016lx",print_vsep,v); }

// array order: low element first (left-to-right)
void avx2_print_8x32 (__m256i x) { wrap_reg_8 (s,x); dump_hex_8 (s[0]); for_range(i,1,32) dump_hex_sep_8 (s[i]); }
void avx2_print_16x16(__m256i x) { wrap_reg_16(s,x); dump_hex_16(s[0]); for_range(i,1,16) dump_hex_sep_16(s[i]); }
void avx2_print_32x8 (__m256i x) { wrap_reg_32(s,x); dump_hex_32(s[0]); for_range(i,1, 8) dump_hex_sep_32(s[i]); }
void avx2_print_64x4 (__m256i x) { wrap_reg_64(s,x); dump_hex_64(s[0]); for_range(i,1, 4) dump_hex_sep_64(s[i]); }

// register order: low element last
// change tick marks except at 128 bit lane boundary
void avx2_reg_print_8x32 (__m256i x) { wrap_reg_8 (s,x); dump_hex_8 (s[31]); for_range(i,1,32) dump_hex_sep_8 (s[31-i]); }
void avx2_reg_print_16x16(__m256i x) { wrap_reg_16(s,x); dump_hex_16(s[15]); for_range(i,1,16) dump_hex_sep_16(s[15-i]); }
void avx2_reg_print_32x8 (__m256i x) { wrap_reg_32(s,x); dump_hex_32(s[ 7]); for_range(i,1, 8) dump_hex_sep_32(s[ 7-i]); }
void avx2_reg_print_64x4 (__m256i x) { wrap_reg_64(s,x); dump_hex_64(s[ 3]); for_range(i,1, 4) dump_hex_sep_64(s[ 3-i]); }


void avx2_print_diff_8x32 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_8(s,x);
  if (s[0]) dump_hex_8(s[0]); else printf("  ");
  for_range(i,1,32) { if(s[i]) dump_hex_sep_8(s[i]); else printf("%s  ", print_vsep); }
}

void avx2_print_diff_16x16 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_16(s,x);
  if (s[0]) dump_hex_16(s[0]); else printf("    ");
  for_range(i,1,16) { if(s[i]) dump_hex_sep_16(s[i]); else printf("%s    ", print_vsep); }
}

void avx2_print_diff_32x8 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_32(s,x);
  if (s[0]) dump_hex_32(s[0]); else printf("        ");
  for_range(i,1,8) { if(s[i]) dump_hex_sep_32(s[i]); else printf("%s        ", print_vsep); }
}

void avx2_print_diff_64x4 (__m256i x, __m256i y)
{
  x = _mm256_xor_si256(x,y);
  wrap_reg_64(s,x);
  if (s[0]) dump_hex_64(s[0]); else printf("                ");
  for_range(i,1,4) { if(s[i]) dump_hex_sep_64(s[i]); else printf("%s                ", print_vsep); }
}


void avx2_dump_ufuncs_8x32_lb(void)
{
  printf("──┴──┴──┴──┴──┴──┴──┴──┴");
  printf("──┴──┴──┴──┴──┴──┴──┴──┴");
  printf("──┴──┴──┴──┴──┴──┴──┴──┴");
  printf("──┴──┴──┴──┴──┴──┴──┴──\n");
  printf("\n");
}

void avx2_dump_ufuncs_16x16_lb(void)
{
  printf("────┴────┴────┴────┴────┴────┴────┴────┴");
  printf("────┴────┴────┴────┴────┴────┴────┴────");
  printf("\n");
}

void avx2_dump_ufuncs_32x8_lb(void)
{
  printf("────────┴────────┴────────┴────────┴");
  printf("────────┴────────┴────────┴────────\n");
}

void avx2_dump_ufuncs_64x4_lb(void)
{
  printf("────────────────┴────────────────┴────────────────┴────────────────\n");
}


void avx2_dump_ufuncs_8x32(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_8x32(  x); printf("\n");
  printf("  ref:"); avx2_print_8x32(ref); printf("\n");
  printf("  out:"); avx2_print_8x32(out); printf("\n");
  printf("      "); avx2_print_diff_8x32(ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_8x32_lb();
}


void avx2_dump_ufuncs_16x16(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_16x16(  x); printf("\n");
  printf("  ref:"); avx2_print_16x16(ref); printf("\n");
  printf("  out:"); avx2_print_16x16(out); printf("\n");
  printf("      "); avx2_print_diff_16x16(ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_16x16_lb();

}

void avx2_dump_ufuncs_32x8(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_32x8(  x); printf("\n");
  printf("  ref:"); avx2_print_32x8(ref); printf("\n");
  printf("  out:"); avx2_print_32x8(out); printf("\n");
//printf("      "); avx2_print_diff_32x8(ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_32x8_lb();
}

void avx2_dump_ufuncs_64x4(__m256i x, __m256i ref, __m256i out)
{
  // meh
  printf("  x:  "); avx2_print_64x4(  x); printf("\n");
  printf("  ref:"); avx2_print_64x4(ref); printf("\n");
  printf("  out:"); avx2_print_64x4(out); printf("\n");
//printf("      "); avx2_print_diff_64x4 (ref,out); printf("\n");
  printf("      "); avx2_dump_ufuncs_64x4_lb();
}

#endif
