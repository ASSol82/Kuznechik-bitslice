//Конвертация работает как в статье CTCrypt, поддерживает пока лишь __m256i
//можно оптимизировать, это не повлияет на алгоритм в статье за счет считывания сразу 4-х векторов


#include <string.h>
#include "Convert.h"
#include "Convert_bitslice.h"
#include "LogicalOperations.h"


static const uint8_t mask[8] = { 0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F };


#define transpose8x8_macros_2(x) \
x = (x & 0xAA55AA55AA55AA55LL) | ((x & 0x00AA00AA00AA00AALL) << 7) | ((x >> 7) & 0x00AA00AA00AA00AALL);\
x = (x & 0xCCCC3333CCCC3333LL) | ((x & 0x0000CCCC0000CCCCLL) << 14) | ((x >> 14) & 0x0000CCCC0000CCCCLL);\
x = (x & 0xF0F0F0F00F0F0F0FLL) | ((x & 0x00000000F0F0F0F0LL) << 28) | ((x >> 28) & 0x00000000F0F0F0F0LL);


// конвертирует биты для bitslice сразу для 8 сообщений
// startNum стартовый бит, также как и в ConvertToBitSlice128, но должен начинаться с бита кратного 8, т.к. записывает сразу байты
inline void ConvertToBitSlice128Transpose8(uint32_t startNum, const uint8_t *src, T dst[128])
{
    int nbyte;
    const uint8_t* p_src = src;
    uint8_t* p = (uint8_t*)dst+(startNum>>3);
    for (nbyte=0;nbyte<16;++nbyte)
    {
        uint64_t w = (uint64_t)p_src[0] | (uint64_t)p_src[16]<<8 | (uint64_t)p_src[32]<<16 | (uint64_t)p_src[48]<<24 | 
        (uint64_t)p_src[64]<<32 | (uint64_t)p_src[80]<<40 | (uint64_t)p_src[96]<<48 | (uint64_t)p_src[112]<<56;
        transpose8x8_macros_2(w);
        p[0]=w; p[sizeof(T)]=(w>>8); p[sizeof(T)*2]=(w>>16); p[sizeof(T)*3]=(w>>24); 
        p[sizeof(T)*4]=(w>>32); p[sizeof(T)*5]=(w>>40); p[sizeof(T)*6]=(w>>48); p[sizeof(T)*7]=(w>>56);
       
        p += (sizeof(T)<<3);
		++p_src;
    }
}


// извлекаем сразу 8 текстов из bitslice векторов
inline void ConvertFromBitSlice128Transpose8(uint32_t startNum, const T *src, uint8_t *dst)
{
    int nbyte;
    uint8_t* p = (uint8_t*)src+(startNum>>3);
    uint8_t* p_dst = dst;
    for (nbyte=0;nbyte<16;++nbyte)
    {   
        uint64_t w = (uint64_t)p[0] | (uint64_t)p[sizeof(T)]<<8 | (uint64_t)p[sizeof(T)*2]<<16 | (uint64_t)p[sizeof(T)*3]<<24 | 
        (uint64_t)p[sizeof(T)*4]<<32 | (uint64_t)p[sizeof(T)*5]<<40 | (uint64_t)p[sizeof(T)*6]<<48 | (uint64_t)p[sizeof(T)*7]<<56;
        transpose8x8_macros_2(w);
        p_dst[0]=w; p_dst[16]=w>>8; p_dst[32]=w>>16; p_dst[48]=w>>24; p_dst[64]=w>>32; p_dst[80]=w>>40; p_dst[96]=w>>48; p_dst[112]=w>>56;
        
		p += (sizeof(T)<<3);
        ++p_dst;
    }
}


void ConvertToBitSlice128(uint32_t num, const uint8_t src[16], T dst[128])
{
	const uint32_t nByte = (num >> 3), nBit = (num & 7);
	for (int i = 0; i < 128; ++i)
	{		
		uint8_t* p = (uint8_t*)(dst + i);
		p[nByte] &= mask[nBit];
		p[nByte] |= (((src[i >> 3] >> (i & 7)) & 1) << nBit);
	}
}


// T src[128] стоит заменить на указатель, т.к. компилятор Visual Studio 2019 может ругаться на большой размер стека,
// компилятор просит по возможности использовать кучу
void ConvertFromBitSlice128(uint32_t num, const T src[128], uint8_t dst[16])
{
	const int nByte = (num >> 3), nBit = (num & 7);

	for (int i = 0; i < 16; i++)
	{
		dst[i] = 0;
		for (int n = 0; n < 8; ++n)
		{
			uint8_t* p = (uint8_t*)&src[(i << 3) + n];
			uint32_t t = (p[nByte] >> nBit) & 1;
			t <<= n;
			dst[i] |= t;
		}
	}
}


#if MaxCountMessage!=256
	//
#elif MaxCountMessage==256


#define transpose8x8_4_macros(x) { \
x = _OR(_OR(_AND(x, c1), _SHIFTL64(_AND(x, c2), 7)), _AND(_SHIFTR64(x, 7), c2)); \
x = _OR(_OR(_AND(x, c3), _SHIFTL64(_AND(x, c4), 14)), _AND(_SHIFTR64(x, 14), c4)); \
x = _OR(_OR(_AND(x, c5), _SHIFTL64(_AND(x, c6), 28)), _AND(_SHIFTR64(x, 28), c6)); }


#define from_8x32_to_256(w256,p32,p0,p1,p2,p3,p4,p5,p6,p7) w256 = _mm256_set_epi64x( \
		(uint64_t)p32[p6]	|	(uint64_t)p32[p7]<<32, \
		(uint64_t)p32[p4]	|	(uint64_t)p32[p5]<<32, \
		(uint64_t)p32[p2]	|	(uint64_t)p32[p3]<<32, \
		(uint64_t)p32[p0]	|	(uint64_t)p32[p1]<<32);


#define ConvertBitSlice_macros(p_src32, p32, of1, offset) \
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


void ConvertToBitSlice128_all(const uint8_t *src, const uint32_t countText, T dst[128])
{
	_ALIGN(32) const __m256i c1 = _mm256_set1_epi64x(0xAA55AA55AA55AA55LL);
	_ALIGN(32) const __m256i c2 = _mm256_set1_epi64x(0x00AA00AA00AA00AALL);
	_ALIGN(32) const __m256i c3 = _mm256_set1_epi64x(0xCCCC3333CCCC3333LL);
	_ALIGN(32) const __m256i c4 = _mm256_set1_epi64x(0x0000CCCC0000CCCCLL);
	_ALIGN(32) const __m256i c5 = _mm256_set1_epi64x(0xF0F0F0F00F0F0F0FLL);
	_ALIGN(32) const __m256i c6 = _mm256_set1_epi64x(0x00000000F0F0F0F0LL);

	_ALIGN(32) const __m256i perm = _mm256_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0, 15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	_ALIGN(32) const __m256i perm8x32 = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);
	
	for (uint32_t i = 0; i < (countText & 0xFFFFFFE0UL); i+=32) // ~7UL); i+=8)
	{
		int nbyte;
		int32_t* p_src32 = (int32_t*)(src+(i<<4));		
		uint32_t* p32 = (uint32_t*)((uint8_t*)dst+(i>>3));
		for (nbyte=0;nbyte<16;nbyte+=4)
		{
			ConvertBitSlice_macros(p_src32, p32, 4, 8);
			p_src32+=1;
			p32 += 256;
		}
	}
	for (uint32_t i = (countText & 0xFFFFFFE0UL); i < countText; ++i) // оставшиеся открытые тексты конвертируем
	{
		ConvertToBitSlice128(i, src + (i << 4), dst); // эту функцию следует вызвать для оставшихся блоков
	}	
}


//можно оптимизировать, считывая по 4 байта в вектора. Ниже работающая версия From
void ConvertFromBitSlice128_all(const T src[128], const uint32_t countText, uint8_t *dst)
{
	_ALIGN(32) const __m256i c1 = _mm256_set1_epi64x(0xAA55AA55AA55AA55LL);
	_ALIGN(32) const __m256i c2 = _mm256_set1_epi64x(0x00AA00AA00AA00AALL);
	_ALIGN(32) const __m256i c3 = _mm256_set1_epi64x(0xCCCC3333CCCC3333LL);
	_ALIGN(32) const __m256i c4 = _mm256_set1_epi64x(0x0000CCCC0000CCCCLL);
	_ALIGN(32) const __m256i c5 = _mm256_set1_epi64x(0xF0F0F0F00F0F0F0FLL);
	_ALIGN(32) const __m256i c6 = _mm256_set1_epi64x(0x00000000F0F0F0F0LL);
	
	//_ALIGN(32) const __m256i perm = _mm256_set_epi32(0x0F0B0703, 0x0E0A0602, 0x0D090501, 0x0C080400, 0x0F0B0703, 0x0E0A0602, 0x0D090501, 0x0C080400);
	_ALIGN(32) const __m256i perm = _mm256_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0, 15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	_ALIGN(32) const __m256i perm8x32 = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);

	for (uint32_t i = 0; i < (countText & 0xFFFFFFE0UL); i+=32)
	{
		uint32_t* p_src32 = (uint32_t*)src + (i>>5);
		uint32_t* p32 = (uint32_t*)(dst + (i<<4));
		int nbyte;
		for (nbyte=0;nbyte<16;nbyte+=4)
		{
			ConvertBitSlice_macros(p_src32, p32, 8, 4);
			p_src32 += 256;
			p32 += 1;
		}
	}
	for (uint32_t i = (countText & 0xFFFFFFE0UL); i < countText; ++i)
	{
		ConvertFromBitSlice128(i, src, dst + (i << 4));
	}
}


#endif
