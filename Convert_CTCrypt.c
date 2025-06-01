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
	
	uint32_t i;
	for (i = 0; i < (countText & 0xFFFFFFE0UL); i+=32) // ~7UL); i+=8)
	{
		int nbyte;
		int32_t* p_src32 = (int32_t*)(src+(i<<4));		
		uint32_t* p32 = (uint32_t*)((uint8_t*)dst+(i>>3));
		for (nbyte=0;nbyte<16;nbyte+=4)
		{
			_ALIGN(32) __m256i w256[4];
			from_8x32_to_256(w256[0],p_src32,0,4,8,12,16,20,24,28);
			from_8x32_to_256(w256[1],p_src32,32,36,40,44,48,52,56,60);
			from_8x32_to_256(w256[2],p_src32,64,68,72,76,80,84,88,92);
			from_8x32_to_256(w256[3],p_src32,96,100,104,108,112,116,120,124);

			w256[0] = _mm256_shuffle_epi8(w256[0],perm); // собираю по 2 байта
			w256[0] = _mm256_permutevar8x32_epi32(w256[0], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
			w256[1] = _mm256_shuffle_epi8(w256[1],perm); // собираю по 2 байта
			w256[1] = _mm256_permutevar8x32_epi32(w256[1], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
			w256[2] = _mm256_shuffle_epi8(w256[2],perm); // собираю по 2 байта
			w256[2] = _mm256_permutevar8x32_epi32(w256[2], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
			w256[3] = _mm256_shuffle_epi8(w256[3],perm); // собираю по 2 байта
			w256[3] = _mm256_permutevar8x32_epi32(w256[3], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе

			transpose8x8_4_macros(w256[0])
			transpose8x8_4_macros(w256[1])
			transpose8x8_4_macros(w256[2])
			transpose8x8_4_macros(w256[3])

			_ALIGN(32) __m256i t1, t2, t3;
			t1 = _mm256_unpacklo_epi8(w256[0], w256[1]); //t1 содержит word из битов в младших 64-х битовых половинках
			t2 = _mm256_unpacklo_epi8(w256[2], w256[3]);
			t3 = _mm256_unpacklo_epi16(t1, t2); // t3 содержит dword из word в младших половинах 128-битовых линий
			p32[0]  = _mm256_extract_epi32(t3, 0);
			p32[8]  = _mm256_extract_epi32(t3, 1);
			p32[16] = _mm256_extract_epi32(t3, 2);
			p32[24] = _mm256_extract_epi32(t3, 3);
			p32[16*8] = _mm256_extract_epi32(t3, 4);
			p32[17*8] = _mm256_extract_epi32(t3, 5);
			p32[18*8] = _mm256_extract_epi32(t3, 6);
			p32[19*8] = _mm256_extract_epi32(t3, 7);
			t3 = _mm256_unpackhi_epi16(t1, t2); // t3 содержит dword из word в старших половинах каждой 128-битовых линий
			p32[32]  = _mm256_extract_epi32(t3, 0);
			p32[40]  = _mm256_extract_epi32(t3, 1);
			p32[48]  = _mm256_extract_epi32(t3, 2);
			p32[56]  = _mm256_extract_epi32(t3, 3);
			p32[20*8]  = _mm256_extract_epi32(t3, 4);
			p32[21*8] = _mm256_extract_epi32(t3, 5);
			p32[22*8] = _mm256_extract_epi32(t3, 6);
			p32[23*8] = _mm256_extract_epi32(t3, 7);
			t1 = _mm256_unpackhi_epi8(w256[0], w256[1]); //t1 содержит word из битов в младших 64-х битовых половинках
			t2 = _mm256_unpackhi_epi8(w256[2], w256[3]);
			t3 = _mm256_unpacklo_epi16(t1, t2); // t3 содержит dword из word в младших половинах 128-битовых линий
			p32[8*8]  = _mm256_extract_epi32(t3, 0);
			p32[9*8]  = _mm256_extract_epi32(t3, 1);
			p32[10*8] = _mm256_extract_epi32(t3, 2);
			p32[11*8] = _mm256_extract_epi32(t3, 3);
			p32[24*8] = _mm256_extract_epi32(t3, 4);
			p32[25*8] = _mm256_extract_epi32(t3, 5);
			p32[26*8] = _mm256_extract_epi32(t3, 6);
			p32[27*8] = _mm256_extract_epi32(t3, 7);
			t3 = _mm256_unpackhi_epi16(t1, t2); // t3 содержит dword из word в младших половинах 128-битовых линий
			p32[12*8]  = _mm256_extract_epi32(t3, 0);
			p32[13*8]  = _mm256_extract_epi32(t3, 1);
			p32[14*8] = _mm256_extract_epi32(t3, 2);
			p32[15*8] = _mm256_extract_epi32(t3, 3);
			p32[28*8] = _mm256_extract_epi32(t3, 4);
			p32[29*8] = _mm256_extract_epi32(t3, 5);
			p32[30*8] = _mm256_extract_epi32(t3, 6);
			p32[31*8] = _mm256_extract_epi32(t3, 7);

			p_src32+=1;
			p32 += 256;
		}
	}
	for (i = (countText & 0xFFFFFFE0UL); i < countText; ++i) // оставшиеся открытые тексты конвертируем
	{
		ConvertToBitSlice128(i, src + (i << 4), dst); // эту функцию можно вызвать если осталось менее 8 текстов
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
	
	//_ALIGN(32) const __m256i perm = _mm256_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0, 15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0);
	//_ALIGN(32) const __m256i perm8x32 = _mm256_set_epi32(7, 6, 3, 2, 5, 4, 1, 0);
	//_ALIGN(32) const __m256i perm_final = _mm256_set_epi8(15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8, 6, 4, 2, 0, 15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8, 6, 4, 2, 0);

	//_ALIGN(32) const __m256i perm = _mm256_set_epi32(0x0F0B0703, 0x0E0A0602, 0x0D090501, 0x0C080400, 0x0F0B0703, 0x0E0A0602, 0x0D090501, 0x0C080400);
	_ALIGN(32) const __m256i perm = _mm256_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0, 15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
	_ALIGN(32) const __m256i perm8x32 = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);

	int i;
	for (i = 0; i < (countText & 0xFFFFFFE0UL); i+=32)
	{
		uint32_t* p_src32 = (uint32_t*)src + (i>>5);
		uint32_t* p32 = (uint32_t*)(dst + (i<<4));
		int nbyte;
		for (nbyte=0;nbyte<16;nbyte+=4)
		{
			_ALIGN(32) __m256i w256[4];
			from_8x32_to_256(w256[0],p_src32,0,8,16,24,32,40,48,56);
			from_8x32_to_256(w256[1],p_src32,64,72,80,88,96,104,112,120);
			from_8x32_to_256(w256[2],p_src32,128,136,144,152,160,168,176,184);
			from_8x32_to_256(w256[3],p_src32,192,200,208,216,224,232,240,248);

			w256[0] = _mm256_shuffle_epi8(w256[0], perm); // собираю по 2 байта
			w256[0] = _mm256_permutevar8x32_epi32(w256[0], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
			w256[1] = _mm256_shuffle_epi8(w256[1], perm); // собираю по 2 байта
			w256[1] = _mm256_permutevar8x32_epi32(w256[1], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
			w256[2] = _mm256_shuffle_epi8(w256[2], perm); // собираю по 2 байта
			w256[2] = _mm256_permutevar8x32_epi32(w256[2], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
			w256[3] = _mm256_shuffle_epi8(w256[3], perm); // собираю по 2 байта
			w256[3] = _mm256_permutevar8x32_epi32(w256[3], perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе

			transpose8x8_4_macros(w256[0])
			transpose8x8_4_macros(w256[1])
			transpose8x8_4_macros(w256[2])
			transpose8x8_4_macros(w256[3])

			_ALIGN(32) __m256i t1, t2, t3;
			t1 = _mm256_unpacklo_epi8(w256[0], w256[1]); //t1 содержит word из битов в младших 64-х битовых половинках
			t2 = _mm256_unpacklo_epi8(w256[2], w256[3]);
			t3 = _mm256_unpacklo_epi16(t1, t2); // t3 содержит dword из word в младших половинах 128-битовых линий
			p32[0 * 4] = _mm256_extract_epi32(t3, 0);
			p32[1 * 4] = _mm256_extract_epi32(t3, 1);
			p32[2 * 4] = _mm256_extract_epi32(t3, 2);
			p32[3 * 4] = _mm256_extract_epi32(t3, 3);
			p32[16 * 4] = _mm256_extract_epi32(t3, 4);
			p32[17 * 4] = _mm256_extract_epi32(t3, 5);
			p32[18 * 4] = _mm256_extract_epi32(t3, 6);
			p32[19 * 4] = _mm256_extract_epi32(t3, 7);
			t3 = _mm256_unpackhi_epi16(t1, t2); // t3 содержит dword из word в старших половинах каждой 128-битовых линий
			p32[4 * 4] = _mm256_extract_epi32(t3, 0);
			p32[5 * 4] = _mm256_extract_epi32(t3, 1);
			p32[6 * 4] = _mm256_extract_epi32(t3, 2);
			p32[7 * 4] = _mm256_extract_epi32(t3, 3);
			p32[20 * 4] = _mm256_extract_epi32(t3, 4);
			p32[21 * 4] = _mm256_extract_epi32(t3, 5);
			p32[22 * 4] = _mm256_extract_epi32(t3, 6);
			p32[23 * 4] = _mm256_extract_epi32(t3, 7);
			t1 = _mm256_unpackhi_epi8(w256[0], w256[1]); //t1 содержит word из битов в младших 64-х битовых половинках
			t2 = _mm256_unpackhi_epi8(w256[2], w256[3]);
			t3 = _mm256_unpacklo_epi16(t1, t2); // t3 содержит dword из word в младших половинах 128-битовых линий
			p32[8 * 4] = _mm256_extract_epi32(t3, 0);
			p32[9 * 4] = _mm256_extract_epi32(t3, 1);
			p32[10 * 4] = _mm256_extract_epi32(t3, 2);
			p32[11 * 4] = _mm256_extract_epi32(t3, 3);
			p32[24 * 4] = _mm256_extract_epi32(t3, 4);
			p32[25 * 4] = _mm256_extract_epi32(t3, 5);
			p32[26 * 4] = _mm256_extract_epi32(t3, 6);
			p32[27 * 4] = _mm256_extract_epi32(t3, 7);
			t3 = _mm256_unpackhi_epi16(t1, t2); // t3 содержит dword из word в младших половинах 128-битовых линий
			p32[12 * 4] = _mm256_extract_epi32(t3, 0);
			p32[13 * 4] = _mm256_extract_epi32(t3, 1);
			p32[14 * 4] = _mm256_extract_epi32(t3, 2);
			p32[15 * 4] = _mm256_extract_epi32(t3, 3);
			p32[28 * 4] = _mm256_extract_epi32(t3, 4);
			p32[29 * 4] = _mm256_extract_epi32(t3, 5);
			p32[30 * 4] = _mm256_extract_epi32(t3, 6);
			p32[31 * 4] = _mm256_extract_epi32(t3, 7);

			p_src32 += 256;
			p32 += 1;
		}
	}
	for (i = (countText & 0xFFFFFFE0UL); i < countText; ++i) //for (i = 0; i < countText; ++i)
	{
		ConvertFromBitSlice128(i, src, dst + (i << 4));
	}
}


//CTCrypt old version
//void ConvertToBitSlice128_all(const uint8_t *src, const uint32_t countText, T dst[128])
//{
//	_ALIGN(32) const __m256i c1 = _mm256_set1_epi64x(0xAA55AA55AA55AA55LL);
//	_ALIGN(32) const __m256i c2 = _mm256_set1_epi64x(0x00AA00AA00AA00AALL);
//	_ALIGN(32) const __m256i c3 = _mm256_set1_epi64x(0xCCCC3333CCCC3333LL);
//	_ALIGN(32) const __m256i c4 = _mm256_set1_epi64x(0x0000CCCC0000CCCCLL);
//	_ALIGN(32) const __m256i c5 = _mm256_set1_epi64x(0xF0F0F0F00F0F0F0FLL);
//	_ALIGN(32) const __m256i c6 = _mm256_set1_epi64x(0x00000000F0F0F0F0LL);
//
////	_ALIGN(32) const __m256i m4 = _mm256_set1_epi64x(0x00000000F0F0F0F0LL);
////	_ALIGN(32) const __m256i m2 = _mm256_set1_epi64x(0x0000CCCC0000CCCCLL);
////	_ALIGN(32) const __m256i m1 = _mm256_set1_epi64x(0x00AA00AA00AA00AALL);
//
//	_ALIGN(32) const __m256i perm = _mm256_set_epi8(15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0,15,14,11,10,7,6,3,2,13,12,9,8,5,4,1,0);
//	_ALIGN(32) const __m256i perm8x32 = _mm256_set_epi32(7,6,3,2,5,4,1,0);
//	_ALIGN(32) const __m256i perm_final = _mm256_set_epi8(15,13,11,9,7,5,3,1,14,12,10,8,6,4,2,0,15,13,11,9,7,5,3,1,14,12,10,8,6,4,2,0);
//
////	_ALIGN(32) const __m256i index = _mm256_set_epi32(28,24,20,16,12,8,4,0);
//
//	uint32_t i;
//	for (i = 0; i < (countText & 0xFFFFFFF8UL); i+=8) // ~7UL); i+=8)
//	{
//		int nbyte;
//		int32_t* p_src32 = (int32_t*)(src+(i<<4));
//		const uint8_t* p8 = (src+(i<<4));
//		uint8_t* p = (uint8_t*)dst+(i>>3);
//		for (nbyte=0;nbyte<16;nbyte+=4)
//		{
////			union union256 w256;
////			w256.u32[0] = p_src32[0]; w256.u32[1] = p_src32[4]; w256.u32[2] = p_src32[8]; w256.u32[3] = p_src32[12];
////			w256.u32[4] = p_src32[16]; w256.u32[5] = p_src32[20]; w256.u32[6] = p_src32[24]; w256.u32[7] = p_src32[28];
////			_ALIGN(32) __m256i w256 = _mm256_i32gather_epi32(p_src32, index, 4);
//			_ALIGN(32) __m256i w256 = _mm256_set_epi64x(
//				(uint64_t)p_src32[24]	|	(uint64_t)p_src32[28]<<32,
//				(uint64_t)p_src32[16]	|	(uint64_t)p_src32[20]<<32,
//				(uint64_t)p_src32[8]	|	(uint64_t)p_src32[12]<<32,
//				(uint64_t)p_src32[0]	|	(uint64_t)p_src32[4]<<32);
//			
////			_ALIGN(32) __m256i w256 = _mm256_set_epi64x(
////				p8x8_to_p64(p8,3,19,35,51,67,83,99,115),
////				p8x8_to_p64(p8,2,18,34,50,66,82,98,114),
////				p8x8_to_p64(p8,1,17,33,49,65,81,97,113),
////				p8x8_to_p64(p8,0,16,32,48,64,80,96,112));
//
////			_ALIGN(32) __m256i w256 = _mm256_set_epi8(
////				p8[115],p8[99],p8[83],p8[67],p8[51],p8[35],p8[19],p8[3],
////				p8[114],p8[98],p8[82],p8[66],p8[50],p8[34],p8[18],p8[2],
////				p8[113],p8[97],p8[81],p8[65],p8[49],p8[33],p8[17],p8[1],
////				p8[112],p8[96],p8[80],p8[64],p8[48],p8[32],p8[16],p8[0]);
//		
//			w256 = _mm256_shuffle_epi8(w256,perm); // собираю по 2 байта
//			w256 = _mm256_permutevar8x32_epi32(w256, perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
//			w256 = _mm256_shuffle_epi8(w256,perm_final); // после этой перестановки - байты 1..8 вектора w256 будут являться первыми байтами сообщений 1..8, байты 9..16 будут являться вторыми байтами сообщений 1..8, байты 17..24 будут являться третьими байтами сообщений 1..8, ...
//
//			transpose8x8_4_macros(w256)
//			_ALIGN(32) uint32_t w[8];
//			_mm256_store_si256((__m256i*)w, w256);
//
//			p[0]=w[0]; 				p[sizeof(T)]=(w[0]>>8);		p[sizeof(T)*2]=(w[0]>>16);	p[sizeof(T)*3]=(w[0]>>24); 
//			p[sizeof(T)*4]=w[1]; 	p[sizeof(T)*5]=(w[1]>>8);	p[sizeof(T)*6]=(w[1]>>16);	p[sizeof(T)*7]=(w[1]>>24);
//			p[sizeof(T)*8]=w[2];	p[sizeof(T)*9]=(w[2]>>8);	p[sizeof(T)*10]=(w[2]>>16);	p[sizeof(T)*11]=(w[2]>>24);
//			p[sizeof(T)*12]=w[3];	p[sizeof(T)*13]=(w[3]>>8);	p[sizeof(T)*14]=(w[3]>>16);	p[sizeof(T)*15]=(w[3]>>24);
//			p[sizeof(T)*16]=w[4];	p[sizeof(T)*17]=(w[4]>>8);	p[sizeof(T)*18]=(w[4]>>16);	p[sizeof(T)*19]=(w[4]>>24);
//			p[sizeof(T)*20]=w[5];	p[sizeof(T)*21]=(w[5]>>8);	p[sizeof(T)*22]=(w[5]>>16);	p[sizeof(T)*23]=(w[5]>>24);
//			p[sizeof(T)*24]=w[6];	p[sizeof(T)*25]=(w[6]>>8);	p[sizeof(T)*26]=(w[6]>>16);	p[sizeof(T)*27]=(w[6]>>24);
//			p[sizeof(T)*28]=w[7];	p[sizeof(T)*29]=(w[7]>>8);	p[sizeof(T)*30]=(w[7]>>16);	p[sizeof(T)*31]=(w[7]>>24);
//
//			p_src32+=1;
//			p8+=4;
//			p += ((sizeof(T)<<3)*4);
//		}
//	}
//	for (i = (countText & 0xFFFFFFF8UL); i < countText; ++i) // оставшиеся открытые тексты конвертируем
//	{
//		ConvertToBitSlice128(i, src + (i << 4), dst); // эту функцию можно вызвать если осталось менее 8 текстов
//	}	
//}


//CTCrypt old version
//void ConvertFromBitSlice128_all(const T src[128], const uint32_t countText, uint8_t *dst)
//{
//	_ALIGN(32) const __m256i c1 = _mm256_set1_epi64x(0xAA55AA55AA55AA55LL);
//	_ALIGN(32) const __m256i c2 = _mm256_set1_epi64x(0x00AA00AA00AA00AALL);
//	_ALIGN(32) const __m256i c3 = _mm256_set1_epi64x(0xCCCC3333CCCC3333LL);
//	_ALIGN(32) const __m256i c4 = _mm256_set1_epi64x(0x0000CCCC0000CCCCLL);
//	_ALIGN(32) const __m256i c5 = _mm256_set1_epi64x(0xF0F0F0F00F0F0F0FLL);
//	_ALIGN(32) const __m256i c6 = _mm256_set1_epi64x(0x00000000F0F0F0F0LL);
//	
//	_ALIGN(32) const __m256i perm = _mm256_set_epi8(15,7,14,6,13,5,12,4,11,3,10,2,9,1,8,0,15,7,14,6,13,5,12,4,11,3,10,2,9,1,8,0);
//	_ALIGN(32) const __m256i perm8x32 = _mm256_set_epi32(7,6,3,2,5,4,1,0);
//	_ALIGN(32) const __m256i perm_final = _mm256_set_epi8(15,14,7,6,13,12,5,4,11,10,3,2,9,8,1,0,15,14,7,6,13,12,5,4,11,10,3,2,9,8,1,0);
//
//	int i;
//	for (i = 0; i < (countText & 0xFFFFFFF8UL); i+=8)
//	{
//		uint8_t* p_src = (uint8_t*)src+(i>>3);
//		uint32_t* p_dst32 = (uint32_t*)(dst+(i<<4));
//		int nbyte;    
//		for (nbyte=0;nbyte<16;nbyte+=4)
//		{
//			//_mm256_set_epi64x(e3,e2,e1,e0)
//			_ALIGN(32) __m256i w256 = _mm256_set_epi64x(
//			(uint64_t)p_src[sizeof(T)*24] | (uint64_t)p_src[sizeof(T)*25]<<8 | (uint64_t)p_src[sizeof(T)*26]<<16 | (uint64_t)p_src[sizeof(T)*27]<<24 |
//			(uint64_t)p_src[sizeof(T)*28]<<32 | (uint64_t)p_src[sizeof(T)*29]<<40 | (uint64_t)p_src[sizeof(T)*30]<<48 | (uint64_t)p_src[sizeof(T)*31]<<56,
//			(uint64_t)p_src[sizeof(T)*16] | (uint64_t)p_src[sizeof(T)*17]<<8 | (uint64_t)p_src[sizeof(T)*18]<<16 | (uint64_t)p_src[sizeof(T)*19]<<24 |
//			(uint64_t)p_src[sizeof(T)*20]<<32 | (uint64_t)p_src[sizeof(T)*21]<<40 | (uint64_t)p_src[sizeof(T)*22]<<48 | (uint64_t)p_src[sizeof(T)*23]<<56,
//			(uint64_t)p_src[sizeof(T)*8] | (uint64_t)p_src[sizeof(T)*9]<<8 | (uint64_t)p_src[sizeof(T)*10]<<16 | (uint64_t)p_src[sizeof(T)*11]<<24 |
//			(uint64_t)p_src[sizeof(T)*12]<<32 | (uint64_t)p_src[sizeof(T)*13]<<40 | (uint64_t)p_src[sizeof(T)*14]<<48 | (uint64_t)p_src[sizeof(T)*15]<<56,
//			(uint64_t)p_src[0] | (uint64_t)p_src[sizeof(T)]<<8 | (uint64_t)p_src[sizeof(T)*2]<<16 | (uint64_t)p_src[sizeof(T)*3]<<24 |
//			(uint64_t)p_src[sizeof(T)*4]<<32 | (uint64_t)p_src[sizeof(T)*5]<<40 | (uint64_t)p_src[sizeof(T)*6]<<48 | (uint64_t)p_src[sizeof(T)*7]<<56
//			);
//
//			transpose8x8_4_macros(w256)
//			w256 = _mm256_shuffle_epi8(w256,perm); // собираю по 2 байта
//			w256 = _mm256_permutevar8x32_epi32(w256, perm8x32); // делаю перестановку среди 32-х битовых переменных, чтобы байты сообщений 1..4 оказались в левом 128-битовом векторе, а байты сообщений 5..8 в правом 128-битовом векторе
//			w256 = _mm256_shuffle_epi8(w256,perm_final); // собираю по 4 байта
//			_ALIGN(32) uint32_t w[8];
//			_mm256_store_si256((__m256i*)w, w256);
//
//			p_dst32[0]=w[0]; p_dst32[4]=w[1];
//			p_dst32[8]=w[2]; p_dst32[12]=w[3];
//			p_dst32[16]=w[4]; p_dst32[20]=w[5];
//			p_dst32[24]=w[6]; p_dst32[28]=w[7];
//
//			p_src += ((sizeof(T)<<3)*4);
//			p_dst32+=1;
//		}
//	}
//	for (i = (countText & 0xFFFFFFF8UL); i < countText; ++i) //for (i = 0; i < countText; ++i)
//	{
//		ConvertFromBitSlice128(i, src, dst + (i << 4));
//	}
//}


#endif
