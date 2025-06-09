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
			ConvertBitslice_32x32_macros(p_src32, p32, 4, 8);
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
			ConvertBitslice_32x32_macros(p_src32, p32, 8, 4);
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
