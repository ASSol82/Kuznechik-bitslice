

#include "VectorType.h"
#include <stdint.h>


void ConvertToBitSlice128Transpose8(uint32_t startNum, const uint8_t *src, T dst[128]);
void ConvertFromBitSlice128Transpose8(uint32_t startNum, const T *src, uint8_t *dst);

// конвертируют все тексты внутри функции, все тексты имеется ввиду <=MaxCountMessage
void ConvertToBitSlice128_all(const uint8_t *src, const uint32_t countText, T dst[128]);
void ConvertFromBitSlice128_all(const T src[128], const uint32_t countText, uint8_t *dst);

/* нулевой бит src записывается в позицию num dst[0], 1-й бит из src записывается в позицию num dst[1], ... 
Размер src в битах должен равняться количеству векторов dst */
void ConvertToBitSlice128(uint32_t num, const uint8_t src[16], T dst[128]);
//void ConvertToBitSlice128_2(uint32_t num, const uint8_t src[16], T dst[128]);

/* обратное преобразование ConvertToBitSlice */
void ConvertFromBitSlice128(uint32_t num, const T src[128], uint8_t dst[16]);
