

#include "LogicalOperations.h"
//#include "VectorType.h"
//#include "Functions.h"
#include "Convert.h"
#include "KeyExpansion.h"
#include "L_tr_bitslice.h"
#include "Subst_tr_bitslice.h"
#include "PiCoordFuncSSE.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>


// наложение 8-ми битов ключа на вектор a, точнее на вектора, т.к. каждый бит ключа влияет на весь вектор a[j+*]
#define byte_xor_bitslice_macros(b,j) \
	a[j]   = (((b) & 0x01) ? _NOT(a[j])   : a[j]);\
	a[j+1] = (((b) & 0x02) ? _NOT(a[j+1]) : a[j+1]);\
	a[j+2] = (((b) & 0x04) ? _NOT(a[j+2]) : a[j+2]);\
	a[j+3] = (((b) & 0x08) ? _NOT(a[j+3]) : a[j+3]);\
	a[j+4] = (((b) & 0x10) ? _NOT(a[j+4]) : a[j+4]);\
	a[j+5] = (((b) & 0x20) ? _NOT(a[j+5]) : a[j+5]);\
	a[j+6] = (((b) & 0x40) ? _NOT(a[j+6]) : a[j+6]);\
	a[j+7] = (((b) & 0x80) ? _NOT(a[j+7]) : a[j+7]);


/*#define byte_xor_bitslice_macros(b,j,mask) \
//	a[j]   = (((b) & (mask)) ? _NOT(a[j])   : a[j]);\
//	a[j+1] = (((b) & ((mask)<<1)) ? _NOT(a[j+1]) : a[j+1]);\
//	a[j+2] = (((b) & ((mask)<<2)) ? _NOT(a[j+2]) : a[j+2]);\
//	a[j+3] = (((b) & ((mask)<<3)) ? _NOT(a[j+3]) : a[j+3]);\
//	a[j+4] = (((b) & ((mask)<<4)) ? _NOT(a[j+4]) : a[j+4]);\
//	a[j+5] = (((b) & ((mask)<<5)) ? _NOT(a[j+5]) : a[j+5]);\
//	a[j+6] = (((b) & ((mask)<<6)) ? _NOT(a[j+6]) : a[j+6]);\
//	a[j+7] = (((b) & ((mask)<<7)) ? _NOT(a[j+7]) : a[j+7]);*/


#define uint64_xor_bitslice_macros(b,j,mask) \
	byte_xor_bitslice_macros(b,j,mask) \
	byte_xor_bitslice_macros(b,j+8,mask<<8) \
	byte_xor_bitslice_macros(b,j+16,mask<<16) \
	byte_xor_bitslice_macros(b,j+24,mask<<24) \
	byte_xor_bitslice_macros(b,j+32,mask<<32) \
	byte_xor_bitslice_macros(b,j+40,mask<<40) \
	byte_xor_bitslice_macros(b,j+48,mask<<48) \
	byte_xor_bitslice_macros(b,j+56,mask<<56)
	

#define byte16_xor_bitslice_macros(key) \
	byte_xor_bitslice_macros(key[0],0)\
	byte_xor_bitslice_macros(key[1],8)\
	byte_xor_bitslice_macros(key[2],16)\
	byte_xor_bitslice_macros(key[3],24)\
	byte_xor_bitslice_macros(key[4],32)\
	byte_xor_bitslice_macros(key[5],40)\
	byte_xor_bitslice_macros(key[6],48)\
	byte_xor_bitslice_macros(key[7],56)\
	byte_xor_bitslice_macros(key[8],64)\
	byte_xor_bitslice_macros(key[9],72)\
	byte_xor_bitslice_macros(key[10],80)\
	byte_xor_bitslice_macros(key[11],88)\
	byte_xor_bitslice_macros(key[12],96)\
	byte_xor_bitslice_macros(key[13],104)\
	byte_xor_bitslice_macros(key[14],112)\
	byte_xor_bitslice_macros(key[15],120)


void xor_bitslice(T a[128], const uint8_t key[16])
{
	byte16_xor_bitslice_macros(key)
}


// countText <= MaxCountMessage
int Kuznechik_bitslice_encrypt_keyexp(const uint8_t key_exp[10][16], uint8_t* text, const uint32_t countText)
{
	uint32_t i;
	//_ALIGN(MALLOCALIGN) 
	T a[128];

	ConvertToBitSlice128_all(text, countText, a);

	for (i = 0; i < 9; ++i)
	{
		xor_bitslice(a, key_exp[i]); //byte16_xor_bitslice_macros(key_exp[i])
		functionPiStroke_SSE_Kuznechik(a);
		L_transform_bitslice(a);
	}
	xor_bitslice(a, key_exp[9]); //byte16_xor_bitslice_macros(key_exp[9])

	ConvertFromBitSlice128_all(a, countText, text);

	return 0;
}


int Kuznechik_bitslice_decrypt_keyexp(const uint8_t key_exp[10][16], uint8_t* text)
{
	int i, j;
	T a[128];
//	uint8_t key_exp[10][16];
//	KeyExpansion(key, key_exp);

	ConvertToBitSlice128(0, text, a);
	//ConvertToBitSlice128_2(0, text, a);

	for (i = 0; i < 9; ++i)
	{
		for (j = 0; j < 128; ++j)
		{
			if ((key_exp[9-i][j >> 3] >> (j & 7)) & 1)
			{
				a[j] = _NOT(a[j]);
				//a[j] = ~a[j];
			}
		}
		//Subst_tr_bitslice(a, 16);
		L_invert_transform_bitslice(a);
		Subst_invert_tr_bitslice(a, 16);
		//xor_tr(text, key_exp[i]);
		//subst_tr(text);
		//L_transform(text);
	}
	for (j = 0; j < 128; ++j)
	{
		if ((key_exp[0][j >> 3] >> (j & 7)) & 1)
		{
			a[j] = _NOT(a[j]);
			//a[j] = ~a[j];
		}
	}
	//xor_tr(text, key_exp[9]);

	ConvertFromBitSlice128(0, a, text);

	return 0;
}


int Kuznechik_bitslice_encrypt_test()
{
	// src_key_test
	const uint8_t src_key_test[] = {
	0xef,0xcd,0xab,0x89,0x67,0x45,0x23,0x01,0x10,0x32,0x54,0x76,0x98,0xba,0xdc,0xfe,
	0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88 };
	uint8_t text[] = { 0x88, 0x99, 0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00,0x77,0x66,0x55,0x44,0x33,0x22,0x11 };
	const uint8_t text_src[] = { 0x88, 0x99, 0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00,0x77,0x66,0x55,0x44,0x33,0x22,0x11 };
	const uint8_t text_enc[] = { 0xCD, 0xED, 0xD4, 0xB9, 0x42, 0x8D, 0x46, 0x5A, 0x30, 0x24, 0xBC, 0xBE, 0x90, 0x9D, 0x67, 0x7F };
	int i;
	uint8_t key_exp[10][16];
	KeyExpansion(src_key_test, key_exp);

	_ALIGN(16) uint64_t key_exp_uint64[10][2];
	for (int i=0;i<10;++i)
	{
		key_exp_uint64[i][0] = key_exp[i][7];
		key_exp_uint64[i][1] = key_exp[i][15];
		for (int j=1;j<8;++j)
		{
			key_exp_uint64[i][0]<<=8; key_exp_uint64[i][1]<<=8;
			key_exp_uint64[i][0] |= key_exp[i][7-j];
			key_exp_uint64[i][1] |= key_exp[i][15-j];
		}
	}

	Kuznechik_bitslice_encrypt_keyexp(key_exp, text, 1);
	//Kuznechik_bitslice_encrypt(src_key_test, text, 1);

	printf("Kuznechik_encrypt test ... ");
	for (i = 0; i < 16; ++i)
	{
		if (text[i] != text_enc[i])
		{
			printf("error\n");
			return -1;
		}
	}
	printf("ok\n");

	Kuznechik_bitslice_decrypt_keyexp(key_exp, text);
	//Kuznechik_bitslice_decrypt(src_key_test, text);
	printf("Kuznechik_decrypt test ... ");
	for (i = 0; i < 16; ++i)
	{
		if (text[i] != text_src[i])
		{
			printf("error\n");
			return -1;
		}
	}
	printf("ok\n");

	return 0;
}


int Kuznechik_bitslice_encrypt_test_3(const uint32_t countEncrypt)
{
	const uint8_t src_key_test[] = {
	0xef,0xcd,0xab,0x89,0x67,0x45,0x23,0x01,0x10,0x32,0x54,0x76,0x98,0xba,0xdc,0xfe,
	0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88 };
	uint8_t* text = malloc(countEncrypt * 16);

	uint32_t n;
	uint32_t temp;
	uint32_t control = 0;

	for (n = 0; n < countEncrypt; ++n)
	{
		const uint32_t pos = n * 16;
		text[pos] = (uint8_t)n;
		text[pos + 1] = (uint8_t)(n >> 8);
		text[pos + 2] = (uint8_t)(n >> 16);
		text[pos + 3] = (uint8_t)(n >> 24);
		text[pos + 4] = 0; text[pos + 5] = 0; text[pos + 6] = 0; text[pos + 7] = 0;
		text[pos + 8] = 0; text[pos + 9] = 0; text[pos + 10] = 0; text[pos + 11] = 0;
		text[pos + 12] = 0; text[pos + 13] = 0; text[pos + 14] = 0; text[pos + 15] = 0;
	}

	_ALIGN(16) uint8_t key_exp[10][16];
	KeyExpansion(src_key_test, key_exp);

	struct timespec startTime3, endTime3;
	timespec_get(&startTime3, TIME_UTC);
	
	for (n = 0; n < countEncrypt; n += MaxCountMessage)
	{
		Kuznechik_bitslice_encrypt_keyexp(key_exp, text + n * 16, MaxCountMessage);
	}

	timespec_get(&endTime3, TIME_UTC);
	double cpuTimeUsed3 = ((double)endTime3.tv_sec - startTime3.tv_sec) + ((double)endTime3.tv_nsec - startTime3.tv_nsec) / 1000000000.0;

	printf("countEncrypt = %u\n", countEncrypt);
	double speed = (double)countEncrypt / 65536.f / cpuTimeUsed3;
	printf("Speed encrypt %f Mbytes/s\n", speed);
	printf("Time elapsed variant timespec_get() %f\n", cpuTimeUsed3);
	
	for (n = 0; n < countEncrypt; ++n)
	{
		const uint32_t pos = n * 16;
		temp = text[pos + 0] ^ text[pos + 4] ^ text[pos + 8] ^ text[pos + 12];
		control ^= temp;
		temp = text[pos + 1] ^ text[pos + 5] ^ text[pos + 9] ^ text[pos + 13];
		control ^= (temp << 8);
		temp = text[pos + 2] ^ text[pos + 6] ^ text[pos + 10] ^ text[pos + 14];
		control ^= (temp << 16);
		temp = text[pos + 3] ^ text[pos + 7] ^ text[pos + 11] ^ text[pos + 15];
		control ^= (temp << 24);
	}

	printf("Control value %08X\n", control);

	free(text);

	return 0;
}


int Kuznechik_bitslice_encrypt_file(const char *fn)
{
	const uint8_t src_key_test[] = {
	0xef,0xcd,0xab,0x89,0x67,0x45,0x23,0x01,0x10,0x32,0x54,0x76,0x98,0xba,0xdc,0xfe,
	0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88 };
	uint32_t n;
	uint32_t temp;
	uint32_t control = 0;
	uint32_t countBlock;

	FILE *f = fopen(fn, "rb");
	if (!f)
	{
		printf("Error fopen\n");
		exit(1); //return -1;
	}
	fseek(f, 0, SEEK_END);
	size_t filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	printf("file size %lu\n", filesize);
	if (!filesize) exit(1);
	
	countBlock = ((filesize - 1) / 16 + 1);

	uint8_t* text = malloc(((countBlock - 1) / MaxCountMessage + 1) * (MaxCountMessage * 16)); // round by (MaxCountMessage * 16) bytes
	size_t r = fread(text, 1, filesize, f);
	if (r != filesize)
	{
		printf("Error fread\n");
		exit(1);
	}
	fclose(f);
	
	printf("file readed\n");

	_ALIGN(16) uint8_t key_exp[10][16];
	KeyExpansion(src_key_test, key_exp);

	struct timespec startTime3, endTime3;
	timespec_get(&startTime3, TIME_UTC);
	
	for (n = 0; n < countBlock; n += MaxCountMessage)
	{
		Kuznechik_bitslice_encrypt_keyexp(key_exp, text + n * 16, MaxCountMessage);
	}

	timespec_get(&endTime3, TIME_UTC);
	double cpuTimeUsed3 = ((double)endTime3.tv_sec - startTime3.tv_sec) + ((double)endTime3.tv_nsec - startTime3.tv_nsec) / 1000000000.0;

	printf("countBlock = %u\n", countBlock);
	double speed = (double)countBlock / 65536.f / cpuTimeUsed3;
	printf("Speed encrypt %f Mbytes/s\n", speed);
	printf("Time elapsed variant timespec_get() %f\n", cpuTimeUsed3);
	
	for (n = 0; n < countBlock; ++n)
	{
		const uint32_t pos = n * 16;
		temp = text[pos + 0] ^ text[pos + 4] ^ text[pos + 8] ^ text[pos + 12];
		control ^= temp;
		temp = text[pos + 1] ^ text[pos + 5] ^ text[pos + 9] ^ text[pos + 13];
		control ^= (temp << 8);
		temp = text[pos + 2] ^ text[pos + 6] ^ text[pos + 10] ^ text[pos + 14];
		control ^= (temp << 16);
		temp = text[pos + 3] ^ text[pos + 7] ^ text[pos + 11] ^ text[pos + 15];
		control ^= (temp << 24);
	}

	printf("Control value %08X\n", control);
	free(text);
	return 0;
}
