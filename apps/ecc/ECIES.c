/**
 * All new code in this distribution is Copyright 2007 by North Carolina
 * State University. All rights reserved. Redistribution and use in
 * source and binary forms are permitted provided that this entire
 * copyright notice is duplicated in all such copies, and that any
 * documentation, announcements, and other materials related to such
 * distribution and use acknowledge that the software was developed at
 * North Carolina State University, Raleigh, NC. No charge may be made
 * for copies, derivations, or distributions of this material without the
 * express written consent of the copyright holder. Neither the name of
 * the University nor the name of the author may be used to endorse or
 * promote products derived from this material without specific prior
 * written permission.
 *
 * IN NO EVENT SHALL THE NORTH CAROLINA STATE UNIVERSITY BE LIABLE TO ANY
 * PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE NORTH CAROLINA STATE UNIVERSITY HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN
 * "AS IS" BASIS, AND THE NORTH CAROLINA STATE UNIVERSITY HAS NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS. "
 *
 */

/**
 * $Id: ECIESM.nc,v 1.5 2007/11/02 22:36:39 aliu3 Exp $
 * Module implement ECIES operations
 */

#include "../cp-abe/sha1.h"
#include <string.h>
#include <ECIES.h>

#define POINT_COMPRESS


#ifdef SLIDING_WIN
static Point baseArray[NUM_POINTS];
#ifdef PROJECTIVE
static ZCoordinate ZList[NUM_POINTS];
#endif
#endif

  //init ECIES
  //initialize the public key pair
  void ECIES_init(){
    Params *p = ECC_get_param();
    get_param(p);
    ECC_init();
  }

  //refer to RFC 2104
static void hmac_sha1(uint8_t *text, int text_len, uint8_t *key, int key_len, uint8_t *digest){
    SHA1Context context;
    uint8_t k_ipad[65];    /* inner padding -
			    * key XORd with ipad
			    */
    uint8_t k_opad[65];    /* outer padding -
			    * key XORd with opad
			    */
    uint8_t tk[20];
    int i;
    /* if key is longer than 64 bytes reset it to key=MD5(key) */
    if (key_len > 64) {
      
      SHA1Context      tctx;
      
      SHA1_Reset(&tctx);
      SHA1_Update(&tctx, key, key_len);
      SHA1_Digest(&tctx, tk);
      
      key = tk;
      key_len = 20;
    }
    
    /*
     * the HMAC_SHA1 transform looks like:
     *
     * SHA1(K XOR opad, SHA1(K XOR ipad, text))
     *
     * where K is an n byte key
     * ipad is the byte 0x36 repeated 64 times
     
     * opad is the byte 0x5c repeated 64 times
     * and text is the data being protected
     */
    
    /* start out by storing key in pads */
    memcpy(k_ipad, key, key_len);
    memset(k_ipad + key_len, 0, 65 - key_len);
    memcpy(k_opad, key, key_len);
    memset(k_opad + key_len, 0, 65 - key_len);
    
    /* XOR key with ipad and opad values */
    for (i=0; i<64; i++) {
      k_ipad[i] ^= 0x36;
      k_opad[i] ^= 0x5c;
    }
    /*
     * perform inner SHA1
     */
    SHA1_Reset(&context);                   /* init context for 1st pass */
    SHA1_Update(&context, k_ipad, 64);      /* start with inner pad */
    SHA1_Update(&context, text, text_len); /* then text of datagram */
    SHA1_Digest(&context, digest);          /* finish up 1st pass */
    /*
     * perform outer SHA1
     */
    SHA1_Reset(&context);                   /* init context for 2nd pass */
    SHA1_Update(&context, k_opad, 64);     /* start with outer pad */
    SHA1_Update(&context, digest, 20);
    SHA1_Digest(&context, digest);         /* then results of 1st hash */

  }  

static void KDF(uint8_t *K, int K_len, uint8_t *Z){
    int len, i;
    uint8_t z[KEYDIGITS*NN_DIGIT_LEN+4];
    SHA1Context ctx;
    uint8_t sha1sum[20];

    memcpy(z, Z, KEYDIGITS*NN_DIGIT_LEN);
    memset(z + KEYDIGITS*NN_DIGIT_LEN, 0, 3);
    //KDF
    //|z|+|ShareInfo|+4 < 2^64, no need to check
    //keydatalen < 20*(2^32-1), no need to check
    len = K_len;
    i = 1;
    while(len > 0){
      z[KEYDIGITS*NN_DIGIT_LEN + 3] = i;
      SHA1_Reset(&ctx);
      SHA1_Update(&ctx, z, KEYDIGITS*NN_DIGIT_LEN+4);
      SHA1_Digest(&ctx, sha1sum);
      if(len >= 20){
	memcpy(K+(i-1)*20, sha1sum, 20);
      }else{
	memcpy(K+(i-1)*20, sha1sum, len);
      }
      i++;
      len = len - 20;
    }
  }
  
  //C - ciphertext
  //M - plaintext, M_len - the length of plaintext <= 61
  //PublicKey - public key of other entity
int ECIES_encrypt(uint8_t *Ct, int C_len, uint8_t *M, int M_len, Point *PublicKey){
    NN_DIGIT k[NUMWORDS];
    uint8_t z[KEYDIGITS*NN_DIGIT_LEN];
    Point R, P;
    //uint8_t octet_buf[2*KEYDIGITS*NN_DIGIT_LEN];
    int octet_len;
    uint8_t K[MAX_M_LEN + HMAC_LEN];
    int i;
#ifdef POINT_COMPRESS
    if(C_len < KEYDIGITS*NN_DIGIT_LEN+1+HMAC_LEN)
      return -1;
#else
    if(C_len < 2*KEYDIGITS*NN_DIGIT_LEN+1+HMAC_LEN)
      return -1;
#endif

    //1. select key pair
#ifdef TEST_VECTOR
#ifdef EIGHT_BIT_PROCESSOR
    k[20] = 0x0;
    k[19] = 0x7b;
    k[18] = 0x01;
    k[17] = 0x2d;
    k[16] = 0xb7;
    k[15] = 0x68;
    k[14] = 0x1a;
    k[13] = 0x3f;
    k[12] = 0x28;
    k[11] = 0xb9;
    k[10] = 0x18;
    k[9] = 0x5c;
    k[8] = 0x8b;
    k[7] = 0x2a;
    k[6] = 0xc5;
    k[5] = 0xd5;
    k[4] = 0x28;
    k[3] = 0xde;
    k[2] = 0xcd;
    k[1] = 0x52;
    k[0] = 0xda;
#elif defined(SIXTEEN_BIT_PROCESSOR)
    k[10] = 0x0;
    k[9] = 0x7b01;
    k[8] = 0x2db7;
    k[7] = 0x681a;
    k[6] = 0x3f28;
    k[5] = 0xb918;
    k[4] = 0x5c8b;
    k[3] = 0x2ac5;
    k[2] = 0xd528;
    k[1] = 0xdecd;
    k[0] = 0x52da;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
    k[5] = 0x0;
    k[4] = 0x7b012db7;
    k[3] = 0x681a3f28;
    k[2] = 0xb9185c8b;
    k[1] = 0x2ac5d528;
    k[0] = 0xdecd52da;
#endif
#else  //random
    ECC_gen_private_key(k);
#endif
    ECC_gen_public_key(&R, k);

    //2. convert R to octet string
#ifdef POINT_COMPRESS
    octet_len = ECC_point2octet(Ct, C_len, &R, TRUE);
#else  //no point compression
    octet_len = ECC_point2octet(Ct, C_len, &R, FALSE);
#endif

    //3. derive shared secret z=P.x
#ifdef SLIDING_WIN
#ifdef PROJECTIVE
    ECC_win_precompute_Z(PublicKey, baseArray, ZList);
    ECC_win_mul_Z(&P, k, baseArray, ZList);
#else
    ECC_win_precompute(PublicKey, baseArray);
    ECC_win_mul(&P, k, baseArray);
#endif //PROJECTIVE
#else  //SLIDING_WIN
    ECC_mul(&P, PublicKey, k);
#endif  //SLIDING_WIN

    if (ECC_point_is_zero(&P))
      return -1;

    //4. convert z to octet string Z
    NNEncode(z, KEYDIGITS*NN_DIGIT_LEN, P.x, NUMWORDS);

    //5. use KDF to generate K of length enckeylen + mackeylen octets from Z
    //enckeylen = M_len, mackeylen = 20
    KDF(K, M_len+HMAC_LEN, z);

    //6. the left most enckeylen octets of K is EK, right most mackeylen octets is MK

    //7. encrypt EM
    for (i=0; i<M_len; i++){
      Ct[octet_len+i] = M[i] ^ K[i];
    }

    //8. generate mac D
    hmac_sha1(Ct + octet_len, M_len, K + M_len, HMAC_LEN, Ct + octet_len + M_len);

    //9. output C = R||EM||D
    return (octet_len + M_len + HMAC_LEN);    

  }

int ECIES_decrypt(uint8_t *M, int M_len, uint8_t *Ct, int C_len, NN_DIGIT *d){

    uint8_t z[KEYDIGITS*NN_DIGIT_LEN];
    Point R, P;
    int octet_len;
    uint8_t K[MAX_M_LEN + HMAC_LEN];
    int i;    
    uint8_t hmac_tmp[HMAC_LEN];

    //1. parse R||EM||D
    
    //2. get the point R
    octet_len = ECC_octet2point(&R, Ct, C_len);

    //3. make sure R is valid
    if (ECC_check_point(&R) != 1)
      return -1;
 
    //4. use private key to generate shared secret z
#ifdef SLIDING_WIN
#ifdef PROJECTIVE
    ECC_win_precompute_Z(&R, baseArray, ZList);
    ECC_win_mul_Z(&P, d, baseArray, ZList);
#else
    ECC_win_precompute(&R, baseArray);
    ECC_win_mul(&P, d, baseArray);
#endif //PROJECTIVE
#else  //SLIDING_WIN
    ECC_mul(&P, &R, d);
#endif  //SLIDING_WIN

    if (ECC_point_is_zero(&P))
      return -1;    

    //5. convert z to octet string Z
    NNEncode(z, KEYDIGITS*NN_DIGIT_LEN, P.x, NUMWORDS);

    //6. use KDF to derive EK and MK
    KDF(K, C_len - octet_len, z);

    //7. check D first
    if (M_len < C_len - HMAC_LEN - octet_len)
      return -1;
    M_len = C_len - HMAC_LEN - octet_len;
    hmac_sha1(Ct + octet_len, M_len, K + M_len, HMAC_LEN, hmac_tmp);

    for (i=0; i<HMAC_LEN; i++){
      if (hmac_tmp[i] != Ct[octet_len + M_len + i])
	return -2;
    }
    
    //8. decrypt
    for(i=0; i<M_len; i++){
      M[i] = Ct[octet_len+i] ^ K[i];
    }

    return M_len;
  }
