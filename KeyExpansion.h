#ifndef _KEY_EXPANSION_
#define _KEY_EXPANSION_


#include <stdint.h>


// ���������� ����� ��� 10 ������� � key[0], key[1] �.�. �������� �������� ����� ��� 1 � 2-�� ������
int KeyExpansion(const uint8_t src_key[32], uint8_t key[10][16]);


int KeyExpansion_test();


#endif //_KEY_EXPANSION_


