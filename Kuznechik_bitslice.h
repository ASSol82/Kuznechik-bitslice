

#include <stdint.h>


int Kuznechik_bitslice_encrypt_keyexp(const uint8_t key_exp[10][16], uint8_t* text, const uint32_t countText);
//int Kuznechik_bitslice_encrypt(const uint8_t key[32], uint8_t* text, const uint32_t countText);
//int Kuznechik_bitslice_decrypt(const uint8_t key[32], uint8_t* text);
int Kuznechik_bitslice_decrypt_keyexp(const uint8_t key_exp[10][16], uint8_t* text);
int Kuznechik_bitslice_encrypt_test();
//int Kuznechik_bitslice_encrypt_test_2(const uint32_t countEncrypt);
int Kuznechik_bitslice_encrypt_test_3(const uint32_t countEncrypt);
int Kuznechik_bitslice_encrypt_file(const char *fn);
