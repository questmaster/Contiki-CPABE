#ifndef _ECDH_H_
#define _ECDH_H_

#include <ECC.h>


//init ECDH module
extern void ECDH_init();
//key agreement, K is the established key
extern int ECDH_key_agree(uint8_t *K, NN_UINT keydatalen, Point *PublicKey, NN_DIGIT *PrivateKey);



#endif
