#ifndef __PICOORDFUNC__
#define __PICOORDFUNC__


#include "VectorType.h"


#define PiStroke_VERSION 21 // определяется версия Pi


#if PiStroke_VERSION==1
#include "PiCoordFuncSSE.1.h"
#elif PiStroke_VERSION==21
#include "PiCoordFuncSSE.2.1.h" // CTCRYPT
#elif PiStroke_VERSION==22
#include "PiCoordFuncSSE.2.2.h" // optimize Inv 18op, Sigma 15op
#elif PiStroke_VERSION==221
#include "PiCoordFuncSSE.2.2.1.h"
#elif PiStroke_VERSION==222
#include "PiCoordFuncSSE.2.2.2.h"
#elif PiStroke_VERSION==23
#include "PiCoordFuncSSE.2.3.h" // LUT
#else
#error Undef PiCoordFuncSSE
#endif


#ifdef __cplusplus
extern "C" {
#endif


int PiVersion();
void pi_r(T* a); // 8 векторов
//void functionPiStroke_SSE(T *a);
void functionPiStroke_SSE_Kuznechik(T* a_);
//void functionPiStroke_SSE(T *a, int countBlock); // кратно 8 векторам, например, для 512 векторов countBlock=64
//void pi_pow_stepbystep(T* a, const uint64_t pow); // 8 векторов, вычисление pi в степени pow шаг за шагом, т.е. без оптимизации. Эта функция для теста подходит, т.к. вычисление степени можно сделать значительно быстрее используя цикличность pi
//#if MaxCountMessage==256
//void PiStroke_bitslice(__m256i *a); // for assembler PiStroke_bitslice_intel.gdb.S
//#endif

#ifdef __cplusplus
}
#endif


#endif
