

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "Functions.h"
//#include "Subst_tr.h"


// 1, a==b, 0, a!=b
int equal_tr(const uint8_t* a, const uint8_t* b)
{
	int i;
	for (i = 0; i < 16; ++i)
	{
		if (a[i] != b[i]) return 0;
	}
	return 1;
}


// a = a XOR b
void xor_tr(uint8_t *a, const uint8_t *b)
{
	int i;
	for (i = 0; i < 16; ++i)
	{
		a[i] ^= b[i];
	}
}


void copy_tr(uint8_t* dst, const uint8_t* src)
{
	int i;
	for (i = 0; i < 16; ++i)
	{
		dst[i] = src[i];
	}
}


void print_a(const uint8_t* a)
{
	int i;
	for (i = 15; i >= 0; --i)
	{
		printf("%02X", a[i]);
	}
}


void print_a_little(const uint8_t* a)
{
	int i;
	for (i = 0; i < 16; ++i)
	{
		if (i) printf(", ");
		printf("0x%02X", a[i]);
	}
}


#ifdef _MSC_VER // для Visual Studio

#include <malloc.h>

void* aligned_malloc(size_t size, uint32_t alignByte) {
	return _aligned_malloc(size, alignByte);
}
void aligned_free(void* ptr) {
	_aligned_free(ptr);
}

#elif defined __GNUC__ //!!! например, для gcc, хотя уточни, там должно быть свое выравнивание

void* aligned_malloc(size_t size, uint32_t alignByte)
{
	return aligned_alloc(alignByte, size);
}
void aligned_free(void *ptr) {
    free(ptr);
}

#else

void* aligned_malloc(size_t size, uint32_t alignByte)
{
	char* mem = malloc(size + alignByte + sizeof(mem));
	if (!mem) return 0;
	char** ptr = (char**)((uintptr_t)(mem + alignByte + sizeof(mem)) & ~(alignByte - 1));
	ptr[-1] = mem;
	return ptr;
}
void aligned_free(void *ptr) {
    free(((char**)ptr)[-1]);
}	
	
#endif
