#ifndef _KEY_EXPANSION_
#define _KEY_EXPANSION_


#include <stdint.h>


// Расширение ключа для 10 раундов в key[0], key[1] д.б. записаны исходные ключи для 1 и 2-го раунда
int KeyExpansion(const uint8_t src_key[32], uint8_t key[10][16]);


int KeyExpansion_test();


#endif //_KEY_EXPANSION_


