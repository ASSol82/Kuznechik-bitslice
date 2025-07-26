#ifndef __CONVERT_BITSLICE__
#define __CONVERT_BITSLICE__


#if MaxCountMessage==256


#include <stdint.h>
#include <immintrin.h>


// __m256i x
#define transpose8x8_4_macros(x) { \
x = _OR(_OR(_AND(x, c1), _SHIFTL64(_AND(x, c2), 7)), _AND(_SHIFTR64(x, 7), c2)); \
x = _OR(_OR(_AND(x, c3), _SHIFTL64(_AND(x, c4), 14)), _AND(_SHIFTR64(x, 14), c4)); \
x = _OR(_OR(_AND(x, c5), _SHIFTL64(_AND(x, c6), 28)), _AND(_SHIFTR64(x, 28), c6)); }


#define from_8x32_to_256(w256,p32,p0,p1,p2,p3,p4,p5,p6,p7) w256 = _mm256_set_epi64x( \
		(uint64_t)p32[p6]	|	(uint64_t)p32[p7]<<32, \
		(uint64_t)p32[p4]	|	(uint64_t)p32[p5]<<32, \
		(uint64_t)p32[p2]	|	(uint64_t)p32[p3]<<32, \
		(uint64_t)p32[p0]	|	(uint64_t)p32[p1]<<32);


#define value_8x32_to_256(w256,v0,v1,v2,v3,v4,v5,v6,v7) w256 = _mm256_set_epi64x( \
		(uint64_t)v6|(uint64_t)v7<<32, (uint64_t)v4|(uint64_t)v5<<32, (uint64_t)v2|(uint64_t)v3<<32,(uint64_t)v0|(uint64_t)v1<<32);


#define ConvertBitslice_32x32_macros(p_src32, p32, of1, offset) \
{ \
	_ALIGN(32) __m256i w256[4]; \
	from_8x32_to_256(w256[0],p_src32,0*of1,1*of1,2*of1,3*of1,4*of1,5*of1,6*of1,7*of1); \
	from_8x32_to_256(w256[1],p_src32,8*of1,9*of1,10*of1,11*of1,12*of1,13*of1,14*of1,15*of1); \
	from_8x32_to_256(w256[2],p_src32,16*of1,17*of1,18*of1,19*of1,20*of1,21*of1,22*of1,23*of1); \
	from_8x32_to_256(w256[3],p_src32,24*of1,25*of1,26*of1,27*of1,28*of1,29*of1,30*of1,31*of1); \
\
	w256[0] = _mm256_shuffle_epi8(w256[0],perm); \
	w256[0] = _mm256_permutevar8x32_epi32(w256[0], perm8x32); \
	w256[1] = _mm256_shuffle_epi8(w256[1],perm); \
	w256[1] = _mm256_permutevar8x32_epi32(w256[1], perm8x32); \
	w256[2] = _mm256_shuffle_epi8(w256[2],perm); \
	w256[2] = _mm256_permutevar8x32_epi32(w256[2], perm8x32); \
	w256[3] = _mm256_shuffle_epi8(w256[3],perm); \
	w256[3] = _mm256_permutevar8x32_epi32(w256[3], perm8x32); \
\
	transpose8x8_4_macros(w256[0]) \
	transpose8x8_4_macros(w256[1]) \
	transpose8x8_4_macros(w256[2]) \
	transpose8x8_4_macros(w256[3]) \
\
	_ALIGN(32) __m256i t1, t2, t3; \
	t1 = _mm256_unpacklo_epi8(w256[0], w256[1]); \
	t2 = _mm256_unpacklo_epi8(w256[2], w256[3]); \
	t3 = _mm256_unpacklo_epi16(t1, t2); \
	p32[0*offset]  = _mm256_extract_epi32(t3, 0); \
	p32[1*offset]  = _mm256_extract_epi32(t3, 1); \
	p32[2*offset] = _mm256_extract_epi32(t3, 2); \
	p32[3*offset] = _mm256_extract_epi32(t3, 3); \
	p32[16*offset] = _mm256_extract_epi32(t3, 4); \
	p32[17*offset] = _mm256_extract_epi32(t3, 5); \
	p32[18*offset] = _mm256_extract_epi32(t3, 6); \
	p32[19*offset] = _mm256_extract_epi32(t3, 7); \
	t3 = _mm256_unpackhi_epi16(t1, t2); \
	p32[4*offset]  = _mm256_extract_epi32(t3, 0); \
	p32[5*offset]  = _mm256_extract_epi32(t3, 1); \
	p32[6*offset]  = _mm256_extract_epi32(t3, 2); \
	p32[7*offset]  = _mm256_extract_epi32(t3, 3); \
	p32[20*offset]  = _mm256_extract_epi32(t3, 4); \
	p32[21*offset] = _mm256_extract_epi32(t3, 5); \
	p32[22*offset] = _mm256_extract_epi32(t3, 6); \
	p32[23*offset] = _mm256_extract_epi32(t3, 7); \
	t1 = _mm256_unpackhi_epi8(w256[0], w256[1]); \
	t2 = _mm256_unpackhi_epi8(w256[2], w256[3]); \
	t3 = _mm256_unpacklo_epi16(t1, t2); \
	p32[8*offset]  = _mm256_extract_epi32(t3, 0); \
	p32[9*offset]  = _mm256_extract_epi32(t3, 1); \
	p32[10*offset] = _mm256_extract_epi32(t3, 2); \
	p32[11*offset] = _mm256_extract_epi32(t3, 3); \
	p32[24*offset] = _mm256_extract_epi32(t3, 4); \
	p32[25*offset] = _mm256_extract_epi32(t3, 5); \
	p32[26*offset] = _mm256_extract_epi32(t3, 6); \
	p32[27*offset] = _mm256_extract_epi32(t3, 7); \
	t3 = _mm256_unpackhi_epi16(t1, t2); \
	p32[12*offset]  = _mm256_extract_epi32(t3, 0); \
	p32[13*offset]  = _mm256_extract_epi32(t3, 1); \
	p32[14*offset] = _mm256_extract_epi32(t3, 2); \
	p32[15*offset] = _mm256_extract_epi32(t3, 3); \
	p32[28*offset] = _mm256_extract_epi32(t3, 4); \
	p32[29*offset] = _mm256_extract_epi32(t3, 5); \
	p32[30*offset] = _mm256_extract_epi32(t3, 6); \
	p32[31*offset] = _mm256_extract_epi32(t3, 7); \
}


#endif //MaxCountMessage==256


#endif //__CONVERT_BITSLICE__
