#ifndef _FUNCTIONS_
#define _FUNCTIONS_


#include <stdint.h>
#include <stddef.h> // для определения size_t


int equal_tr(const uint8_t* a, const uint8_t* b);
void xor_tr(uint8_t* a, const uint8_t* b);
void copy_tr(uint8_t* dst, const uint8_t* src);
void subst_tr(uint8_t* a);
void subst_invert_tr(uint8_t* a);


void print_a(const uint8_t* a);
void print_a_little(const uint8_t* a);


#if defined _MSC_VER
#define _ALIGN(x) __declspec(align(x))
#else
#define _ALIGN(x) __attribute__ ((__aligned__(x)))
#endif


//#ifdef __cplusplus
//extern "C" {
//#endif


void *aligned_malloc(size_t size, uint32_t alignByte);
void aligned_free(void *ptr);


//#ifdef __cplusplus
//}
//#endif


#endif //_FUNCTIONS_

