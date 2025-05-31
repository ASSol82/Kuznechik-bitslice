#ifndef __L_TR_BITSLICE__
#define __L_TR_BITSLICE__


void L_transform_bitslice_prepare();
void L_transform_bitslice(T *v); //T v[128]
void L_invert_transform_bitslice(T v[128]);
int L_transform_bitslice_test();


#endif
