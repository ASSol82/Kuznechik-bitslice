// Îñíîâíîé ôàéë ïðè îïðåäåëåíèè òèïà âåêòîðà T, ò.å. ñþäà ïîìèìî ñòàíäàðòíûõ òèïîâ ïîäêëþ÷àåòñÿ VectorType_SSE.h âåêòîðà äëèíû 128, 256 áèò


#ifndef __VECTOR_TYPE__
#define __VECTOR_TYPE__


#include <stdint.h>
#include <immintrin.h> // ðàáîòàåò ñ AVX2, ò.å. ñ __m256i. äëÿ AVX-512 âêëþ÷èòü -mavx512f -mavx512cd -mavx512vl -mavx512bw -mavx512dq -mavx512ifma -mavx512vbmi
#include "Settings.h"
//#include <stdint.h>
//#include "VectorType_SSE.h"


//#if MaxCountMessage!=64 && MaxCountMessage!=32 && MaxCountMessage!=16 && MaxCountMessage!=8
//#error MaxCountMessage = 64,32,16,8
//#endif


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
