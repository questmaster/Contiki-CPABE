#ifndef _ECIES_H_
#define _ECIES_H_

#include <ECC.h>

#define MAX_M_LEN 41
#define HMAC_LEN 20

//init ECDH module
extern void ECIES_init();

//C - ciphertext
//M - plaintext to be encrypted
//M_len - length of M
//PublicKey - be used to encrypt M
//return the length of C
#ifdef CODE_SIZE
	extern int ECIES_encrypt(uint8_t *Ct, int C_len, uint8_t *M, int M_len, Point *PublicKey) __attribute__ ((noinline));
	
	extern int ECIES_decrypt(uint8_t *M, int M_len, uint8_t *Ct, int C_len, NN_DIGIT *d) __attribute__ ((noinline));
#else
	extern int ECIES_encrypt(uint8_t *Ct, int C_len, uint8_t *M, int M_len, Point *PublicKey);

	extern int ECIES_decrypt(uint8_t *M, int M_len, uint8_t *Ct, int C_len, NN_DIGIT *d);
#endif

#endif
