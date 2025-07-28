

#include "LogicalOperations.h"
//#include "VectorType.h"
//#include "Functions.h"
#include "Convert.h"
//#include "KeyExpansion.h"
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


//	int i, j;
//	uint8_t t[16];
//
//	for (i = 0; i < 16; ++i)
//	{
//		key[0][i] = src_key[16 + i];
//		key[1][i] = src_key[i];
//	}
//
//	for (i = 0; i < 4; ++i)
//	{
//		for (uint32_t k = 0; k < 16; ++k)
//		{
//			key[(i + 1) * 2][k] = key[i * 2][k];
//			key[(i + 1) * 2 + 1][k] = key[i * 2 + 1][k];
//		}
//		for (j = 0; j < 8; ++j)
//		{
//			for (uint32_t k = 0; k < 16; ++k)
//			{
//				t[k] = key[(i + 1) * 2 + (j&1)][k];
//			}			
//			XSL_transform(C_const[i * 8 + j], t);
//			//xor_tr(t, C_const[i * 8 + j]);
//			//subst_tr(t);
//			//L_transform(t);
//			for (uint32_t k = 0; k < 16; ++k)
//			{
//				key[(i + 1) * 2 + 1 - (j&1)][k] ^= t[k];
//			}			
//		}
//	}
int KeyExpansion_bitslice(const uint8_t src_key[32], uint8_t key[10][16])
{
	static const uint8_t C_const[32][16] = { {1, 148, 132, 221, 16, 189, 39, 93, 184, 122, 72, 108, 114, 118, 162, 110}, {2, 235, 203, 121, 32, 185, 78, 186, 179, 244, 144, 216, 228, 236, 135, 220}, {3, 127, 79, 164, 48, 4, 105, 231, 11, 142, 216, 180, 150, 154, 37, 178}, {4, 21, 85, 242, 64, 177, 156, 183, 165, 43, 227, 115, 11, 27, 205, 123}, {5, 129, 209, 47, 80, 12, 187, 234, 29, 81, 171, 31, 121, 109, 111, 21}, {6, 254, 158, 139, 96, 8, 210, 13, 22, 223, 115, 171, 239, 247, 74, 167}, {7, 106, 26, 86, 112, 181, 245, 80, 174, 165, 59, 199, 157, 129, 232, 201}, {8, 42, 170, 39, 128, 161, 251, 173, 137, 86, 5, 230, 22, 54, 89, 246}, {9, 190, 46, 250, 144, 28, 220, 240, 49, 44, 77, 138, 100, 64, 251, 152}, {10, 193, 97, 94, 160, 24, 181, 23, 58, 162, 149, 62, 242, 218, 222, 42}, {11, 85, 229, 131, 176, 165, 146, 74, 130, 216, 221, 82, 128, 172, 124, 68}, {12, 63, 255, 213, 192, 16, 103, 26, 44, 125, 230, 149, 29, 45, 148, 141}, {13, 171, 123, 8, 208, 173, 64, 71, 148, 7, 174, 249, 111, 91, 54, 227}, {14, 212, 52, 172, 224, 169, 41, 160, 159, 137, 118, 77, 249, 193, 19, 81}, {15, 64, 176, 113, 240, 20, 14, 253, 39, 243, 62, 33, 139, 183, 177, 63}, {16, 84, 151, 78, 195, 129, 53, 153, 209, 172, 10, 15, 44, 108, 178, 47}, {17, 192, 19, 147, 211, 60, 18, 196, 105, 214, 66, 99, 94, 26, 16, 65}, {18, 191, 92, 55, 227, 56, 123, 35, 98, 88, 154, 215, 200, 128, 53, 243}, {19, 43, 216, 234, 243, 133, 92, 126, 218, 34, 210, 187, 186, 246, 151, 157}, {20, 65, 194, 188, 131, 48, 169, 46, 116, 135, 233, 124, 39, 119, 127, 84}, {21, 213, 70, 97, 147, 141, 142, 115, 204, 253, 161, 16, 85, 1, 221, 58}, {22, 170, 9, 197, 163, 137, 231, 148, 199, 115, 121, 164, 195, 155, 248, 136}, {23, 62, 141, 24, 179, 52, 192, 201, 127, 9, 49, 200, 177, 237, 90, 230}, {24, 126, 61, 105, 67, 32, 206, 52, 88, 250, 15, 233, 58, 90, 235, 217}, {25, 234, 185, 180, 83, 157, 233, 105, 224, 128, 71, 133, 72, 44, 73, 183}, {26, 149, 246, 16, 99, 153, 128, 142, 235, 14, 159, 49, 222, 182, 108, 5}, {27, 1, 114, 205, 115, 36, 167, 211, 83, 116, 215, 93, 172, 192, 206, 107}, {28, 107, 104, 155, 3, 145, 82, 131, 253, 209, 236, 154, 49, 65, 38, 162}, {29, 255, 236, 70, 19, 44, 117, 222, 69, 171, 164, 246, 67, 55, 132, 204}, {30, 128, 163, 226, 35, 40, 28, 57, 78, 37, 124, 66, 213, 173, 161, 126}, {31, 20, 39, 63, 51, 149, 59, 100, 246, 95, 52, 46, 167, 219, 3, 16}, {32, 168, 237, 156, 69, 193, 106, 241, 97, 155, 20, 30, 88, 216, 167, 94} };
	T a[128+128+128], *t=a+256; // K_{2i+1},K_{2i+2}
	
	for (uint32_t i = 0; i < 16; ++i)
	{
		key[0][i] = src_key[16 + i];
		key[1][i] = src_key[i];
	}

	ConvertToBitSlice128(0, &key[0][0], a);
	ConvertToBitSlice128(0, &key[1][0], a+128);
	
	for (uint32_t i = 0; i < 4; ++i)
	{
		for (uint32_t j = 0; j < 8; ++j)
		{
			for (uint32_t k=0;k<128;++k)
			{
				t[k] = (a+128*(j&1))[k];
			}
			
			xor_bitslice(t, C_const[i*8+j]);
			functionPiStroke_SSE_Kuznechik(t);
			L_transform_bitslice(t);
			for (uint32_t k=0;k<128;++k)
			{
				(a+128*((j+1)&1))[k] = _XOR(t[k], (a+128*((j+1)&1))[k]);
			}
		}

		ConvertFromBitSlice128(0, a, &key[2*(i+1)][0]);
		ConvertFromBitSlice128(0, a+128, &key[2*(i+1)+1][0]);
	}
	
	return 0;
}


int KeyExpansion_bitslice_test()
{
	const uint8_t src_key_test[] = {
	0xef,0xcd,0xab,0x89,0x67,0x45,0x23,0x01,0x10,0x32,0x54,0x76,0x98,0xba,0xdc,0xfe,
	0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88 };

	const uint8_t keyexp_test[10][16] = {
	{ 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88 },
	{ 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01, 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE },
	{ 0x44, 0x8C, 0xC7, 0x8C, 0xEF, 0x6A, 0x8D, 0x22, 0x43, 0x43, 0x69, 0x15, 0x53, 0x48, 0x31, 0xDB },
	{ 0x04, 0xFD, 0x9F, 0x0A, 0xC4, 0xAD, 0xEB, 0x15, 0x68, 0xEC, 0xCF, 0xE9, 0xD8, 0x53, 0x45, 0x3D },
	{ 0xAC, 0xF1, 0x29, 0xF4, 0x46, 0x92, 0xE5, 0xD3, 0x28, 0x5E, 0x4A, 0xC4, 0x68, 0x64, 0x64, 0x57 },
	{ 0x1B, 0x58, 0xDA, 0x34, 0x28, 0xE8, 0x32, 0xB5, 0x32, 0x64, 0x5C, 0x16, 0x35, 0x94, 0x07, 0xBD },
	{ 0xB1, 0x98, 0x00, 0x5A, 0x26, 0x27, 0x57, 0x70, 0xDE, 0x45, 0x87, 0x7E, 0x75, 0x40, 0xE6, 0x51 },
	{ 0x84, 0xF9, 0x86, 0x22, 0xA2, 0x91, 0x2A, 0xD7, 0x3E, 0xDD, 0x9F, 0x7B, 0x01, 0x25, 0x79, 0x5A },
	{ 0x17, 0xE5, 0xB6, 0xCD, 0x73, 0x2F, 0xF3, 0xA5, 0x23, 0x31, 0xC7, 0x78, 0x53, 0xE2, 0x44, 0xBB },
	{ 0x43, 0x40, 0x4A, 0x8E, 0xA8, 0xBA, 0x5D, 0x75, 0x5B, 0xF4, 0xBC, 0x16, 0x74, 0xDD, 0xE9, 0x72 } };

	uint8_t key[10][16];
	KeyExpansion_bitslice(src_key_test, key);

	printf("KeyExpansion_bitslice test ... ");
	for (uint32_t j = 0; j < 10; ++j)
	{
		for (uint32_t k=0;k<16;++k)
		{
			if (key[j][k] != keyexp_test[j][k])
			{
				printf("error\n");
				return -1;				
			}
		}
	}
	printf("ok\n");
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
	KeyExpansion_bitslice(src_key_test, key_exp);
	//KeyExpansion(src_key_test, key_exp);

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
	KeyExpansion_bitslice(src_key_test, key_exp);
	//KeyExpansion(src_key_test, key_exp);

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
	KeyExpansion_bitslice(src_key_test, key_exp);
	//KeyExpansion(src_key_test, key_exp);

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
