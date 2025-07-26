

#include <stdio.h>
#include <stdint.h>
//#include "Functions.h"
#include "VectorType.h"
#include "Convert.h"
#include "L_tr_bitslice.h"
#include "Kuznechik_bitslice.h"
#include "PiCoordFuncSSE.h"


int main(int argc, char *argv[])
{
	char *fn = NULL;
	if (argc > 1)
	{
		fn = argv[1];
	}

	printf("PiVersion %u\n", PiVersion());

	Kuznechik_bitslice_encrypt_test();

	const uint32_t countTestMessage = 1<<24;
	if (fn == NULL)
		Kuznechik_bitslice_encrypt_test_3(countTestMessage); // countEncrypt = 16777216 Control value E92A1D61
	else			
		Kuznechik_bitslice_encrypt_file(fn); // this only demonstration

	return 0;
}
