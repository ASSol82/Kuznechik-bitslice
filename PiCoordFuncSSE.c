//#include "VectorTypeSSE.h"
#include "PiCoordFuncSSE.h"


int PiVersion()
{
	return PiStroke_ver;
}


void pi_r(T* a)
{
	pi_r_macros(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
}


//void functionPiStroke_SSE(T *a_, const int countBlock) // работает медленно
//{	
//	int i;
//	for (i = 0; i < countBlock*8; i += 8)
//	{
//		pi_r_macros(a_[i], a_[i+1], a_[i+2], a_[i+3], a_[i+4], a_[i+5], a_[i+6], a_[i+7]);
//	}
//}


void functionPiStroke_SSE(T *a_)
{	
	int i;
	for (i = 0; i < 512; i += 8)
	{
		pi_r_macros(a_[i], a_[i+1], a_[i+2], a_[i+3], a_[i+4], a_[i+5], a_[i+6], a_[i+7]);
	}
}


//#if MaxCountMessage!=256
void functionPiStroke_SSE_Kuznechik(T* a_)
{
	int i;
	for (i = 0; i < 128; i += 8)
	{
		//PiStroke_bitslice(a_+i);
		pi_r_macros(a_[i], a_[i + 1], a_[i + 2], a_[i + 3], a_[i + 4], a_[i + 5], a_[i + 6], a_[i + 7]);
	}
}
//#else
//void functionPiStroke_SSE_Kuznechik(T* a_)
//{
//	int i;
//	for (i = 0; i < 128; i += 8)
//	{
//		PiStroke_bitslice(a_+i);
//		//pi_r_macros(a_[i], a_[i + 1], a_[i + 2], a_[i + 3], a_[i + 4], a_[i + 5], a_[i + 6], a_[i + 7]);
//	}
//}
//#endif


//медленнее работает, чем с циклом
//void functionPiStroke_SSE_Kuznechik(T* a_)
//{
//	pi_r_macros(a_[0], a_[1], a_[2], a_[3], a_[4], a_[5], a_[6], a_[7]);
//	pi_r_macros(a_[8], a_[9], a_[10], a_[11], a_[12], a_[13], a_[14], a_[15]);
//	pi_r_macros(a_[16], a_[17], a_[18], a_[19], a_[20], a_[21], a_[22], a_[23]);
//	pi_r_macros(a_[24], a_[25], a_[26], a_[27], a_[28], a_[29], a_[30], a_[31]);
//	pi_r_macros(a_[32], a_[33], a_[34], a_[35], a_[36], a_[37], a_[38], a_[39]);
//	pi_r_macros(a_[40], a_[41], a_[42], a_[43], a_[44], a_[45], a_[46], a_[47]);
//	pi_r_macros(a_[48], a_[49], a_[50], a_[51], a_[52], a_[53], a_[54], a_[55]);
//	pi_r_macros(a_[56], a_[57], a_[58], a_[59], a_[60], a_[61], a_[62], a_[63]);
//	pi_r_macros(a_[64], a_[65], a_[66], a_[67], a_[68], a_[69], a_[70], a_[71]);
//	pi_r_macros(a_[72], a_[73], a_[74], a_[75], a_[76], a_[77], a_[78], a_[79]);
//	pi_r_macros(a_[80], a_[81], a_[82], a_[83], a_[84], a_[85], a_[86], a_[87]);
//	pi_r_macros(a_[88], a_[89], a_[90], a_[91], a_[92], a_[93], a_[94], a_[95]);
//	pi_r_macros(a_[96], a_[97], a_[98], a_[99], a_[100], a_[101], a_[102], a_[103]);
//	pi_r_macros(a_[104], a_[105], a_[106], a_[107], a_[108], a_[109], a_[110], a_[111]);
//	pi_r_macros(a_[112], a_[113], a_[114], a_[115], a_[116], a_[117], a_[118], a_[119]);
//	pi_r_macros(a_[120], a_[121], a_[122], a_[123], a_[124], a_[125], a_[126], a_[127]);
//}


void pi_pow_stepbystep(T* a, const uint64_t pow)
{
	uint64_t i;
	for (i = 0; i < pow; ++i)
	{
		pi_r_macros(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
	}
}
