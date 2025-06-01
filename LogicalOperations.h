// Основной файл при определении типа вектора T, т.е. сюда помимо стандартных типов подключается VectorType_SSE.h вектора длины 128, 256 бит


#ifndef __LOGICAL_OPERATIONS__
#define __LOGICAL_OPERATIONS__


#include "Settings.h"
#include <stdint.h>
#include "VectorType.h"


//Для данной ситуации макросы пока не расписаны
#if MaxCountMessage==512

#define MALLOCALIGN 64
#define _ZERORIGHT _mm512_setzero_si512()
#define _ZERO(a) ((a) = _mm512_setzero_si512())
#define _XOR(a,b) _mm512_xor_si512((a), (b))
#define _NOT(a) _mm512_xor_si512((a), _mm512_set1_epi32(0xFFFFFFFF))
#define _OR(a,b) _mm512_or_si512((a), (b))
#define _AND(a,b) _mm512_and_si512((a), (b))
#define _ANDNOT(a,b) _mm512_andnot_si512((a), (b))
#define _SHIFTR64(a,count) _mm512_srli_epi64(a, count) // сдвиг в сторону младших разрядов в рамках 64-х битовых отрезков
#define _SHIFTL64(a,count) _mm512_slli_epi64(a, count) // сдвиг в сторону старших разрядов в рамках 64-х битовых отрезков
#define IsZero(av_) (_mm512_cmp_epi32_mask((av_), _ZERORIGHT, _CMP_EQ_OQ) == 0xFFFF) //  _mm512_cmp_ps_mask((av_), _ZERORIGHT, _CMP_EQ_OQ) // пробую 

#define _LUT3(a,b,c,imm8) _mm512_ternarylogic_epi64((a),(b),(c),imm8)
#define _XORAND(a,b,c) _LUT3(a,b,c,0x6A) // (a&b)^c
#define _NOTXORAND(a,b,c) _LUT3(a,b,c,0x95) // ~((a&b)^c)
#define _ORAND(a,b,c) _LUT3(a,b,c,0xEA) // (a&b)|c
#define _XOROR(a,b,c) _LUT3(a,b,c,0x56) // (a|b)^c

#elif MaxCountMessage==256

#define MALLOCALIGN 32
#define _ZERORIGHT _mm256_setzero_si256() // _mm256_set1_epi32(0)
#define _FFRIGHT _mm256_set1_epi32(0xFFFFFFFF)
#define _ZERO(a) (a) = _mm256_setzero_si256() // _mm256_set1_epi32(0) // _mm256_setzero_si256()
//#define _ONERIGHT _mm256_set1_epi32(0xFFFFFFFF)
#define _XOR(a,b) _mm256_xor_si256((a), (b))
#define _NOT(a) _mm256_xor_si256((a), _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFFULL))
#define _OR(a,b) _mm256_or_si256((a), (b))
#define _AND(a,b) _mm256_and_si256((a), (b))
#define _ANDNOT(a,b) _mm256_andnot_si256((a), (b))
#define _SHIFTR64(a,count) _mm256_srli_epi64(a, count) // сдвиг в сторону младших разрядов в рамках 64-х битовых отрезков
#define _SHIFTL64(a,count) _mm256_slli_epi64(a, count) // сдвиг в сторону старших разрядов в рамках 64-х битовых отрезков
#define IsZero(v) _mm256_testz_si256((v), (v))

#if _TERNARY_LOGIC==1
#define _LUT3(a,b,c,imm8) _mm256_ternarylogic_epi64((a),(b),(c),imm8)
#define _XORAND(a,b,c) _LUT3(a,b,c,0x6A) // (a&b)^c
#define _NOTXORAND(a,b,c) _LUT3(a,b,c,0x95) // ~((a&b)^c)
#define _ORAND(a,b,c) _LUT3(a,b,c,0xEA) // (a&b)|c
#define _XOROR(a,b,c) _LUT3(a,b,c,0x56) // (a|b)^c
#endif

#elif MaxCountMessage==128

#define MALLOCALIGN 16
#define _ZERORIGHT _mm_setzero_si128() //_mm_set1_epi32(0)
#define _FFRIGHT _mm_set1_epi32(0xFFFFFFFF)
#define _ZERO(a) ((a) = _mm_set1_epi32(0))
//#define _ONERIGHT _mm_set1_epi32(0xFFFFFFFF)
#define _XOR(a,b) _mm_xor_si128((a), (b))
#define _NOT(a) _mm_xor_si128((a), _mm_set1_epi32(0xFFFFFFFF))
#define _OR(a,b) _mm_or_si128((a), (b))
#define _AND(a,b) _mm_and_si128((a), (b))
#define _ANDNOT(a,b) _mm_andnot_si128((a), (b))
#define IsZero(v) _mm_testz_si128((v), (v))

#elif (MaxCountMessage<128)

#define MALLOCALIGN 8
#define _ZERORIGHT 0
#define _FFRIGHT (~(T)0)
#define _ZERO(a) ((a) = 0)
//#define _ONERIGHT ~0
#define _XOR(a,b) ((a) ^ (b))
#define _NOT(a) (~(a))
#define _OR(a,b) ((a) | (b))
#define _AND(a,b) ((a) & (b))
#define _ANDNOT(a,b) (~(a) & (b))
#define IsZero(a) ((a)==0)

#endif


#endif // __LOGICAL_OPERATIONS__
