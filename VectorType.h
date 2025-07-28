

#ifndef __VECTOR_TYPE__
#define __VECTOR_TYPE__


#include <stdint.h>
#include <immintrin.h>
#include "Settings.h"


#if MaxCountMessage==512
typedef __m512i T;
#elif MaxCountMessage==256
typedef __m256i T;
#elif MaxCountMessage==128
typedef __m128i T;
#elif MaxCountMessage==64
typedef uint64_t T;
#elif MaxCountMessage==32
typedef uint32_t T;
#elif MaxCountMessage==16
typedef uint16_t T;
#elif MaxCountMessage==8
typedef uint8_t T;
#endif


#endif // !__VECTOR_TYPE__
