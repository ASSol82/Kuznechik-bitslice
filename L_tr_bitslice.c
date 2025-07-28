

#include <stdio.h>
#include "LogicalOperations.h"
#include "Convert.h"


#define L_tr_macros(s) \
c[0] = _XOR(a[(s + 43) & 127], a[(s + 91) & 127]); c[1] = _XOR(a[(s + 9) & 127], a[(s + 121) & 127]); c[2] = _XOR(a[(s + 51) & 127], a[(s + 83) & 127]); c[3] = _XOR(a[(s + 22) & 127], a[(s + 39) & 127]); c[4] = _XOR(a[(s + 54) & 127], a[(s + 86) & 127]); c[5] = _XOR(a[(s + 64) & 127], a[(s + 93) & 127]); c[6] = _XOR(a[(s + 118) & 127], c[0]); c[3] = _XOR(a[(s + 103) & 127], c[3]); c[3] = _XOR(c[6], c[3]); c[6] = _XOR(a[(s + 44) & 127], a[(s + 92) & 127]); c[5] = _XOR(a[(s + 45) & 127], c[5]); c[7] = _XOR(a[(s + 46) & 127], a[(s + 94) & 127]); c[8] = _XOR(a[(s + 23) & 127], c[6]); c[9] = _XOR(a[(s + 29) & 127], c[2]); c[10] = _XOR(a[(s + 53) & 127], c[5]); c[11] = _XOR(a[(s + 47) & 127], a[(s + 95) & 127]); c[12] = _XOR(a[(s + 52) & 127], a[(s + 84) & 127]); c[13] = _XOR(a[(s + 55) & 127], a[(s + 87) & 127]); c[14] = _XOR(a[(s + 68) & 127], a[(s + 70) & 127]); c[9] = _XOR(a[(s + 109) & 127], c[9]); c[15] = _XOR(a[(s + 15) & 127], a[(s + 127) & 127]); c[8] = _XOR(a[(s + 119) & 127], c[8]); c[14] = _XOR(c[1], c[14]); c[10] = _XOR(a[(s + 85) & 127], c[10]); c[16] = _XOR(a[(s + 69) & 127], c[3]); c[7] = _XOR(c[4], c[7]); c[16] = _XOR(a[(s + 71) & 127], c[16]); c[17] = _XOR(a[(s + 19) & 127], a[(s + 36) & 127]); c[18] = _XOR(a[(s + 25) & 127], c[9]); c[19] = _XOR(a[(s + 41) & 127], a[(s + 89) & 127]); c[20] = _XOR(a[(s + 28) & 127], a[(s + 108) & 127]); c[21] = _XOR(a[(s + 65) & 127], c[7]); c[22] = _XOR(a[(s + 30) & 127], c[12]); c[23] = _XOR(a[(s + 31) & 127], c[10]); c[24] = _XOR(a[(s + 42) & 127], a[(s + 90) & 127]); c[17] = _XOR(a[(s + 100) & 127], c[17]); c[22] = _XOR(a[(s + 110) & 127], c[22]); c[25] = _XOR(a[(s + 66) & 127], c[11]); c[26] = _XOR(a[(s + 67) & 127], c[16]); c[18] = _XOR(a[(s + 105) & 127], c[18]); c[17] = _XOR(a[(s + 115) & 127], c[17]); c[27] = _XOR(c[13], c[15]); c[23] = _XOR(a[(s + 111) & 127], c[23]); c[28] = _XOR(a[(s + 14) & 127], a[(s + 126) & 127]); c[25] = _XOR(c[14], c[25]); c[29] = _XOR(a[(s + 20) & 127], a[(s + 37) & 127]); c[30] = _XOR(a[(s + 11) & 127], a[(s + 123) & 127]); c[31] = _XOR(a[(s + 21) & 127], a[(s + 102) & 127]); c[32] = _XOR(a[(s + 12) & 127], a[(s + 124) & 127]); c[33] = _XOR(a[(s + 26) & 127], a[(s + 106) & 127]); c[34] = _XOR(a[(s + 27) & 127], a[(s + 107) & 127]); c[35] = _XOR(a[(s + 13) & 127], a[(s + 125) & 127]); c[36] = _XOR(a[(s + 8) & 127], a[(s + 120) & 127]); c[37] = _XOR(a[(s + 38) & 127], a[(s + 117) & 127]); c[38] = _XOR(a[(s + 40) & 127], c[17]); c[39] = _XOR(a[(s + 49) & 127], a[(s + 81) & 127]); c[40] = _XOR(a[(s + 50) & 127], c[20]); c[41] = _XOR(a[(s + 101) & 127], c[8]); c[42] = _XOR(a[(s + 82) & 127], c[24]); c[29] = _XOR(a[(s + 116) & 127], c[29]); c[43] = _XOR(a[(s + 10) & 127], c[21]); c[44] = _XOR(a[(s + 24) & 127], c[27]); c[44] = _XOR(a[(s + 104) & 127], c[44]); c[33] = _XOR(c[22], c[33]); c[36] = _XOR(c[26], c[36]); c[34] = _XOR(c[23], c[34]); c[39] = _XOR(c[19], c[39]); c[29] = _XOR(c[41], c[29]); c[31] = _XOR(c[31], c[37]); c[30] = _XOR(c[25], c[30]); c[37] = _XOR(a[(s + 122) & 127], c[43]); c[40] = _XOR(c[40], c[42]); c[38] = _XOR(a[(s + 88) & 127], c[38]); c[41] = _XOR(a[(s + 48) & 127], c[18]); c[42] = _XOR(c[28], c[33]); c[41] = _XOR(a[(s + 80) & 127], c[41]); c[28] = _XOR(c[28], c[38]); c[8] = _XOR(c[8], c[30]); c[8] = _XOR(c[35], c[8]); c[43] = _XOR(c[32], c[29]); c[45] = _XOR(c[36], c[40]); c[28] = _XOR(c[31], c[28]); c[42] = _XOR(c[37], c[42]); c[28] = _XOR(c[34], c[28]); c[45] = _XOR(c[44], c[45]); \
a[s + 0] = _XOR(a[s + 0], _XOR(a[(s + 56) & 127], _XOR(a[(s + 64) & 127], _XOR(a[(s + 72) & 127], _XOR(c[3], _XOR(c[14], _XOR(c[20], _XOR(c[18], _XOR(c[17], _XOR(c[44], _XOR(c[39], _XOR(c[29], c[42]))))))))))));\
a[s + 1] = _XOR(a[s + 1], _XOR(a[(s + 57) & 127], _XOR(a[(s + 73) & 127], _XOR(c[2], _XOR(c[7], _XOR(c[16], _XOR(c[39], _XOR(c[30], _XOR(c[40], c[28])))))))));\
a[s + 2] = _XOR(a[s + 2], _XOR(a[(s + 58) & 127], _XOR(a[(s + 70) & 127], _XOR(a[(s + 74) & 127], _XOR(c[11], _XOR(c[12], _XOR(c[9], _XOR(c[19], _XOR(c[37], _XOR(c[43], c[45]))))))))));\
a[s + 3] = _XOR(a[s + 3], _XOR(a[(s + 59) & 127], _XOR(a[(s + 71) & 127], _XOR(a[(s + 75) & 127], _XOR(c[0], _XOR(c[13], _XOR(c[10], _XOR(c[24], _XOR(c[22], _XOR(c[18], _XOR(c[31], c[8])))))))))));\
a[s + 4] = _XOR(a[s + 4], _XOR(a[(s + 32) & 127], _XOR(a[(s + 60) & 127], _XOR(a[(s + 76) & 127], _XOR(a[(s + 96) & 127], _XOR(c[6], _XOR(c[23], _XOR(c[32], _XOR(c[36], c[42])))))))));\
a[s + 5] = _XOR(a[s + 5], _XOR(a[(s + 16) & 127], _XOR(a[(s + 33) & 127], _XOR(a[(s + 61) & 127], _XOR(a[(s + 77) & 127], _XOR(a[(s + 97) & 127], _XOR(a[(s + 112) & 127], _XOR(c[21], _XOR(c[27], _XOR(c[34], c[8]))))))))));\
a[s + 6] = _XOR(a[s + 6], _XOR(a[(s + 17) & 127], _XOR(a[(s + 34) & 127], _XOR(a[(s + 62) & 127], _XOR(a[(s + 78) & 127], _XOR(a[(s + 98) & 127], _XOR(a[(s + 113) & 127], _XOR(c[5], _XOR(c[15], _XOR(c[26], _XOR(c[25], _XOR(c[33], _XOR(c[39], _XOR(c[38], _XOR(c[41], c[43])))))))))))))));\
a[s + 7] = _XOR(a[s + 7], _XOR(a[(s + 18) & 127], _XOR(a[(s + 35) & 127], _XOR(a[(s + 63) & 127], _XOR(a[(s + 79) & 127], _XOR(a[(s + 99) & 127], _XOR(a[(s + 114) & 127], _XOR(c[1], _XOR(c[4], _XOR(c[35], _XOR(c[41], _XOR(c[28], c[45]))))))))))));


void L_transform_bitslice(T *a) //T v[128]
{
	//_ALIGN(MALLOCALIGN) 
	T c[50];
	L_tr_macros(0) L_tr_macros(8) L_tr_macros(16) L_tr_macros(24) L_tr_macros(32) L_tr_macros(40) L_tr_macros(48) L_tr_macros(56)
	L_tr_macros(64) L_tr_macros(72) L_tr_macros(80) L_tr_macros(88) L_tr_macros(96) L_tr_macros(104) L_tr_macros(112) L_tr_macros(120)
}


void L_invert_transform_bitslice(T a[128])
{
	_ALIGN(MALLOCALIGN) T c[50];
	L_tr_macros(120) L_tr_macros(112) L_tr_macros(104) L_tr_macros(96) L_tr_macros(88) L_tr_macros(80) L_tr_macros(72) L_tr_macros(64)
	L_tr_macros(56) L_tr_macros(48) L_tr_macros(40) L_tr_macros(32) L_tr_macros(24) L_tr_macros(16) L_tr_macros(8) L_tr_macros(0)
}
