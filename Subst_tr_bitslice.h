#ifndef __SUBST_BITSLICE__
#define __SUBST_BITSLICE__


#include "VectorType.h"


#ifdef __cplusplus
extern "C" {
#endif


// Подстановка аналогичная подстановке в Стрибог
void Subst_tr_bitslice(T *a, int countBlock);
void Subst_invert_tr_bitslice(T* a, int countBlock);
//void functionPiStroke(T a_[512]);


#ifdef __cplusplus
}
#endif


#endif
