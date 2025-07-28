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


void functionPiStroke_SSE(T *a_)
{	
	int i;
	for (i = 0; i < 512; i += 8)
	{
		pi_r_macros(a_[i], a_[i+1], a_[i+2], a_[i+3], a_[i+4], a_[i+5], a_[i+6], a_[i+7]);
	}
}


void functionPiStroke_SSE_Kuznechik(T* a_)
{
	int i;
	for (i = 0; i < 128; i += 8)
	{
		//PiStroke_bitslice(a_+i);
		pi_r_macros(a_[i], a_[i + 1], a_[i + 2], a_[i + 3], a_[i + 4], a_[i + 5], a_[i + 6], a_[i + 7]);
	}
}
