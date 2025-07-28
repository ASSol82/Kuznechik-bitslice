

#include <stdint.h>


int KeyExpansion_bitslice(const uint8_t src_key[32], uint8_t key[10][16]);
int KeyExpansion_bitslice_test();
int Kuznechik_bitslice_encrypt_keyexp(const uint8_t key_exp[10][16], uint8_t* text, const uint32_t countText);
//int Kuznechik_bitslice_encrypt(const uint8_t key[32], uint8_t* text, const uint32_t countText);
//int Kuznechik_bitslice_decrypt(const uint8_t key[32], uint8_t* text);
int Kuznechik_bitslice_decrypt_keyexp(const uint8_t key_exp[10][16], uint8_t* text);
int Kuznechik_bitslice_encrypt_test();
//int Kuznechik_bitslice_encrypt_test_2(const uint32_t countEncrypt);
int Kuznechik_bitslice_encrypt_test_3(const uint32_t countEncrypt);
int Kuznechik_bitslice_encrypt_file(const char *fn);
