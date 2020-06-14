/*
 *  cp-abe.c
 *  
 *
 *  Created by Daniel Jacobi on 19.05.10.
 *  Copyright 2010 TU Darmstadt. All rights reserved.
 *
 */

#include <stdio.h> // only DEBUG!
#include <string.h>
#include <watchdog.h>
#include <memb.h>
#include "include/cp-abe.h"
#include "TP.h"
#include "qsort.h"
#include "sha1.h"

static TPParams param;
static cpabe_policy_t* cur_comp_pol;
static uint8_t init_done = 0;

unsigned long mem_count = 0;
unsigned long memb_comp_count = 0;
unsigned long memb_policy_count = 0;
unsigned long memb_poly_count = 0;
unsigned long mem_free_count = 0;
unsigned long memb_comp_free_count = 0;
unsigned long memb_policy_free_count = 0;
unsigned long memb_poly_free_count = 0;

MEMB(prv_comps_m, cpabe_prv_comp_t, 8);						/**< This limits the number of attributes in the private key */
MEMB(enc_policy_m, cpabe_policy_t, 8);						/**< This limits the number of attributes in encrypted data */
MEMB(enc_polynomial_m, cpabe_polynomial_t, 8);				/**< This limits the number of attributes in encrypted data */

/* -- Utility functions ----------------------------------------------------- */

/*
 * @brief These two are needed for the list to work with my setup.
 */ 
#define MYLIST(name) \
name = (list_t)&LIST_CONCAT(name,_list)
struct list {
	struct list *next;
};

/*
 * @brief Return item at position index.
 */
void * list_index(list_t list, uint8_t index) {
	struct list *l;
	uint8_t n = 0;
	
	for(l = *list; l != NULL; l = l->next) {
		if (n != index) {
			++n;
		} else {
			break;
		}
		
	}
	
	return l;
} 


/*
 * @brief Splits a String at " ".
 */
static char **strsplit(char *s) {
	char ** tokens = NULL;
	uint8_t i;
	uint8_t tok_count = 1;
	char * tmp;
	char * tmp1;

	// count number of tokens
	for (i = 0; i < strlen(s); i++) {
		if (s[i] == ' ')
			tok_count++;
	}
	
	// alloc base array
	tmp = (char *) malloc((tok_count+1) * sizeof(char *) + strlen(s) + 1);
	tokens = (char **) tmp;
	tmp = tmp + ((tok_count+1) * sizeof(char *));
	memcpy(tmp, s, strlen(s) + 1);
							  
	// reuse string?
	tokens[0] = tmp;
	for (i = 1; i < tok_count; i++) {
		tmp1 = strchr(tmp, ' ');						/**< get pos of next ' ' char */
		if (tmp1 != NULL) {
			*tmp1 = '\0';								/**< replace ' ' by '\0', tmp points to rest of string */
			tmp = tmp1 + 1;
		}
		
		tokens[i] = tmp;
	}
	tokens[i] = 0; // needed to stop the while loop in parser

	return tokens;
}

/**
 * @brief frees all memory used in toks
 */
static void freesplit(char ** toks) {
//	int8_t i = sizeof(toks) / sizeof(char*);
//	for (; i >= 0 ; i--) {
//		free(toks[i]);
//	}
	free(toks);
}


/**
 * This replaces an sscanf call and parses the format "%dto%d".
 */
static uint8_t sscanf_repl(char* tok, /*char * format,*/ uint8_t* k, uint8_t* n) {
	uint16_t i, pos = 0;
	char* param1;
	char* param2;
	uint8_t ret = 0;

	// find 'of'
	for (i = 0; i < strlen(tok)-1; i++) {
		if (tok[i] == 'o' && tok[i+1] == 'f') {
			pos = i;
			break;
		}
	}
	
	if (pos > 0) {
		param1 = (char *) malloc(pos+1);
		param2 = (char *) malloc(strlen(tok) - pos - 2);
		if (param1 == NULL || param2 == NULL) {
			if (param1 == NULL) { 
				free(param2);
			} else if (param2 == NULL) { 
				free(param1);
			}
			// ERR: no mem
			return 0;
		}		
		
		// extract data
		memcpy(param1, tok, sizeof(param1));
		memcpy(param2, tok + pos + 2, sizeof(param2));
	
		// parse param 1
		*k = atoi(param1);
	
		// parse param 2
		*n = atoi(param2);

		// check conversion	
		if (*k != 0) {
			ret++;
		}
		if (*n != 0) {
			ret++;
		}

		free(param1);
		free(param2);
	}
	
	return ret;
}

#ifdef CPABE_DEBUG

void debug_print(char* name, NN_DIGIT *val) {
	int i;
	
	printf("%s: ", name);
	for (i = NUMWORDS-1; i >= 0; i--) {
#ifndef THIRTYTWO_BIT_PROCESSOR
		printf("%x ",val[i]);
#else
		printf("%x %x ", (uint16_t)(val[i] >> 16), (uint16_t) val[i]);
#endif
	}
	printf("\n");
	
}

#endif

/* -- CP-ABE functions ------------------------------------------------------ */

/**
 * @brief init memory structures needed for CP-ABE.
 */
void cpabe_init() {
	if (!init_done) {
		memb_init(&prv_comps_m);
		memb_init(&enc_policy_m);
		memb_init(&enc_polynomial_m);
		init_done = 1;
	}
}

/* ** CP-ABE Setup ********************* */

#ifdef CPABE_SETUP
/**
 * @brief generate public and master key pair for CP-ABE.
 *
 * @param pub Public key
 * @param msk Master key
 */
/* Nice to have (pbc-lib debug mode with cpabe192k2 curve)
 alpha: 66 11 d3 ea 4b 3b 88 81 7 6c 71 49 9e 53 45 ae 
 msk->beta: 51 d6 84 b6 92 7b e1 96 9a 9b f5 69 d5 0 7d 62 
 pub->g.x 14 a7 e9 bd f7 7a 1e 18 d1 e1 e 6 6b 63 0 44 3 b 83 b 8b 37 48 69 
 pub->g.y 14 92 7 c 79 7c ee 4b 75 eb 52 44 da 95 c6 1b 3c af d1 8a b2 4 d0 54 
 pub->gp.x 6 9f 1f 36 80 7d 98 5a b 80 b7 65 98 68 cf 55 b9 c7 2d 57 98 fe 51 9e 
 pub->gp.y b d9 a5 b9 f5 86 a3 49 1a 6 77 42 35 74 81 a 2c d4 12 79 23 bc 5d f9 
 msk->g_alpha.x 1c aa fc 3f 69 8f d0 b 7 a5 ff 5f 3a 2c 36 45 41 f3 ee 91 3c 56 16 48 
 msk->g_alpha.y 18 df 57 e8 30 ea 8f 2d 81 72 d8 d2 99 f1 c8 61 a7 f5 7d aa fe c5 7e 5c 
 pub->h.x 10 4b f2 f1 ee d8 bf 57 a ec 83 8c b 7c 9 b3 5d 6e 3f ca 3f a2 0 74 
 pub->h.y 6 67 a1 17 88 2a da e2 c 66 35 28 e8 8e 4d d5 98 3 68 d8 bb a2 e 35 
 pub->g_hat_alpha.r 3 47 3f 7f c4 b e3 14 c5 f5 c4 70 d8 c3 32 3c 6d 14 b3 90 f2 d3 b0 6c 
 pub->g_hat_alpha.i 11 e a6 b7 75 fb a8 92 f1 19 4f b1 3 14 d6 8 46 cb 68 b8 59 e8 3c 73 
 */
void cpabe_setup(cpabe_pub_t *pub, cpabe_msk_t *msk) {
	// ERROR if NULL pointer 
	if (pub == NULL || msk == NULL) {
//			printf("cpabe_setup - ERROR: pub or msk NULL! pub: %p, msk: %p", pub, msk);
	}
	
	NN_DIGIT alpha[NUMWORDS];

	// init ECC with curve params
	Params *p = ECC_get_param();
	get_param(p);
    ECC_init();
	
	// init TP with curve params
	get_TP_param(&param);
	
#ifdef CPABE_DEBUG
#ifdef SIXTEEN_BIT_PROCESSOR
	alpha[12] = 0x0000;
	alpha[11] = 0x0000;
	alpha[10] = 0x0000;
	alpha[9] = 0x0000;
	alpha[8] = 0x0000;
	alpha[7] = 0x6611;
	alpha[6] = 0xd3ea;
	alpha[5] = 0x4b3b;
	alpha[4] = 0x8881;
	alpha[3] = 0x076c;
	alpha[2] = 0x7149;
	alpha[1] = 0x9e53;
	alpha[0] = 0x45ae;

	msk->beta[12] = 0x0000;
	msk->beta[11] = 0x0000;
	msk->beta[10] = 0x0000;
	msk->beta[9] = 0x0000;
	msk->beta[8] = 0x0000;
	msk->beta[7] = 0x51d6;
	msk->beta[6] = 0x84b6;
	msk->beta[5] = 0x927b;
	msk->beta[4] = 0xe196;
	msk->beta[3] = 0x9a9b;
	msk->beta[2] = 0xf569;
	msk->beta[1] = 0xd500;
	msk->beta[0] = 0x7d62;

	pub->g.x[12] = 0x0000;
	pub->g.x[11] = 0x14a7;
	pub->g.x[10] = 0xe9bd;
	pub->g.x[9] = 0xf77a;
	pub->g.x[8] = 0x1e18;
	pub->g.x[7] = 0xd1e1;
	pub->g.x[6] = 0x0e06;
	pub->g.x[5] = 0x6b63;
	pub->g.x[4] = 0x0044;
	pub->g.x[3] = 0x030b;
	pub->g.x[2] = 0x830b;
	pub->g.x[1] = 0x8b37;
	pub->g.x[0] = 0x4869;
	pub->g.y[12] = 0x0000;
	pub->g.y[11] = 0x1492;
	pub->g.y[10] = 0x070c;
	pub->g.y[9] = 0x797c;
	pub->g.y[8] = 0xee4b;
	pub->g.y[7] = 0x75eb;
	pub->g.y[6] = 0x5244;
	pub->g.y[5] = 0xda95;
	pub->g.y[4] = 0xc61b;
	pub->g.y[3] = 0x3caf;
	pub->g.y[2] = 0xd18a;
	pub->g.y[1] = 0xb204;
	pub->g.y[0] = 0xd054;

	pub->gp.x[12] = 0x0000;
	pub->gp.x[11] = 0x069f;
	pub->gp.x[10] = 0x1f36;
	pub->gp.x[9] = 0x807d;
	pub->gp.x[8] = 0x985a;
	pub->gp.x[7] = 0x0b80;
	pub->gp.x[6] = 0xb765;
	pub->gp.x[5] = 0x9868;
	pub->gp.x[4] = 0xcf55;
	pub->gp.x[3] = 0xb9c7;
	pub->gp.x[2] = 0x2d57;
	pub->gp.x[1] = 0x98fe;
	pub->gp.x[0] = 0x519e;
	pub->gp.y[12] = 0x0000;
	pub->gp.y[11] = 0x0bd9;
	pub->gp.y[10] = 0xa5b9;
	pub->gp.y[9] = 0xf586;
	pub->gp.y[8] = 0xa349;
	pub->gp.y[7] = 0x1a06;
	pub->gp.y[6] = 0x7742;
	pub->gp.y[5] = 0x3574;
	pub->gp.y[4] = 0x810a;
	pub->gp.y[3] = 0x2cd4;
	pub->gp.y[2] = 0x1279;
	pub->gp.y[1] = 0x23bc;
	pub->gp.y[0] = 0x5df9;
#endif
#ifdef THIRTYTWO_BIT_PROCESSOR
#ifdef CPABE192K2
	alpha[6] = 0x00000000;
	alpha[5] = 0x00000000;
	alpha[4] = 0x00000000;
	alpha[3] = 0x6611d3ea;
	alpha[2] = 0x4b3b8881;
	alpha[1] = 0x076c7149;
	alpha[0] = 0x9e5345ae;

	msk->beta[6] = 0x00000000;
	msk->beta[5] = 0x00000000;
	msk->beta[4] = 0x00000000;
	msk->beta[3] = 0x51d684b6;
	msk->beta[2] = 0x927be196;
	msk->beta[1] = 0x9a9bf569;
	msk->beta[0] = 0xd5007d62;

	pub->g.x[6] = 0x00000000;
	pub->g.x[5] = 0x14a7e9bd;
	pub->g.x[4] = 0xf77a1e18;
	pub->g.x[3] = 0xd1e10e06;
	pub->g.x[2] = 0x6b630044;
	pub->g.x[1] = 0x030b830b;
	pub->g.x[0] = 0x8b374869;
	pub->g.y[6] = 0x00000000;
	pub->g.y[5] = 0x1492070c;
	pub->g.y[4] = 0x797cee4b;
	pub->g.y[3] = 0x75eb5244;
	pub->g.y[2] = 0xda95c61b;
	pub->g.y[1] = 0x3cafd18a;
	pub->g.y[0] = 0xb204d054;

	pub->gp.x[6] = 0x00000000;
	pub->gp.x[5] = 0x069f1f36;
	pub->gp.x[4] = 0x807d985a;
	pub->gp.x[3] = 0x0b80b765;
	pub->gp.x[2] = 0x9868cf55;
	pub->gp.x[1] = 0xb9c72d57;
	pub->gp.x[0] = 0x98fe519e;
	pub->gp.y[6] = 0x00000000;
	pub->gp.y[5] = 0x0bd9a5b9;
	pub->gp.y[4] = 0xf586a349;
	pub->gp.y[3] = 0x1a067742;
	pub->gp.y[2] = 0x3574810a;
	pub->gp.y[1] = 0x2cd41279;
	pub->gp.y[0] = 0x23bc5df9;
#endif //CPABE192K2
#endif // THIRTYTWO_BIT_PROCESSOR
#else
	NNModRandom(alpha, param.m, NUMWORDS);			/**< Random alpha */
 	NNModRandom(msk->beta, param.m, NUMWORDS);		/**< Random beta */
	ECC_Random_PointMul(&(pub->g));		
	ECC_Random_PointMul(&(pub->gp));
#endif
	
	ECC_mul(&(msk->g_alpha), &(pub->gp), alpha);	/**< g_alpha = gp * alpha */
	ECC_mul(&(pub->h), &(pub->g), msk->beta);		/**< h = g * beta */

	TP_TatePairing(&(pub->g_hat_alpha), &(pub->g), &(msk->g_alpha));	/**< g_hat_alpha = e(g, gp * alpha) */
}
#endif

/* ** CP-ABE Keygen ********************* */

/**
 * @brief Compute a Point related to a string.
 * @param h Resulting point
 * @param s String for point computation
 */
void point_from_string( Point* h, char* s )
{
	NN_DIGIT r[NUMWORDS/*SHA1HashSize*/]; // result is fixed 20bytes!
	SHA1Context context;
//	NN_DIGIT tmp[NUMWORDS];
	
	NNAssignZero(r, NUMWORDS);
	
	SHA1_Reset(&context);
	SHA1_Update(&context, (uint8_t *) s, strlen(s));
	SHA1_Digest(&context, (uint8_t *)r);

#ifdef CPABE_DEBUG	
	printf("(not used in debug!) attr: \"%s\" -> SHA-1 hash: ", s);
	debug_print("", r);
	
#ifdef SIXTEEN_BIT_PROCESSOR
	if (strcmp(s, "attr1") == 0) {
  	 h->x[12] = 0x0000; // v- this is just for the first attribute!
	 h->x[11] = 0x13c4;
	 h->x[10] = 0xdf84;
	 h->x[9] = 0xa081;
	 h->x[8] = 0xe68c;
	 h->x[7] = 0xd561;
	 h->x[6] = 0x472c;
	 h->x[5] = 0xd660;
	 h->x[4] = 0x9239;
	 h->x[3] = 0x66bd;
	 h->x[2] = 0x282f;
	 h->x[1] = 0xc0c4;
	 h->x[0] = 0x2a98;
	 h->y[12] = 0x0000;
	 h->y[11] = 0x040e;
	 h->y[10] = 0xf170;
	 h->y[9] = 0x911c;
	 h->y[8] = 0xff08;
	 h->y[7] = 0x252f;
	 h->y[6] = 0xae8c;
	 h->y[5] = 0xc809;
	 h->y[4] = 0xec62;
	 h->y[3] = 0x1377;
	 h->y[2] = 0x2d4b;
	 h->y[1] = 0x78c6;
	 h->y[0] = 0xad16;
	} else {
		if (strcmp(s, "attr2") == 0) {
			h->x[12] = 0x0000; // v- this is just for the first attribute!
			h->x[11] = 0x03ad;
			h->x[10] = 0xfc87;
			h->x[9] = 0x32f0;
			h->x[8] = 0x3c7f;
			h->x[7] = 0x5162;
			h->x[6] = 0xde0e; 
			h->x[5] = 0x2bcf;
			h->x[4] = 0x927c;
			h->x[3] = 0x4b1b;
			h->x[2] = 0x8d17;
			h->x[1] = 0x1d54;
			h->x[0] = 0x2056;
			h->y[12] = 0x0000;
			h->y[11] = 0x0dad;
			h->y[10] = 0x9ea8;
			h->y[9] = 0x0e12;
			h->y[8] = 0x94e3;
			h->y[7] = 0x2a54;
			h->y[6] = 0xb901;
			h->y[5] = 0xe120;
			h->y[4] = 0xd7cf;
			h->y[3] = 0x1c10;
			h->y[2] = 0x61ba;
			h->y[1] = 0x3d6c;
			h->y[0] = 0x31a5;
		} else {
			if (strcmp(s, "attr3") == 0) {
				h->x[12] = 0x0000; // v- this is just for the first attribute!
				h->x[11] = 0x1289;
				h->x[10] = 0xd47d;
				h->x[9] = 0x1286;
				h->x[8] = 0x35de;
				h->x[7] = 0xfc1f;
				h->x[6] = 0xbbd3;
				h->x[5] = 0x0d5f;
				h->x[4] = 0x16b8;
				h->x[3] = 0xad97;
				h->x[2] = 0x2566;
				h->x[1] = 0xefae;
				h->x[0] = 0xc851;
				h->y[12] = 0x0000;
				h->y[11] = 0x0891;
				h->y[10] = 0xb7a1;
				h->y[9] = 0xde45;
				h->y[8] = 0xa812;
				h->y[7] = 0xd2b8;
				h->y[6] = 0x9f46;
				h->y[5] = 0xc97c;
				h->y[4] = 0x9962;
				h->y[3] = 0x0fe7;
				h->y[2] = 0xaea4;
				h->y[1] = 0x652b;
				h->y[0] = 0x3238;
			} 
		}
	}
#endif
#ifdef THIRTYTWO_BIT_PROCESSOR
#ifdef CPABE192K2
	if (strcmp(s, "attr1") == 0) {
		h->x[6] = 0x00000000; // v- this is just for the first attribute!
		h->x[5] = 0x13c4df84;
		h->x[4] = 0xa081e68c;
		h->x[3] = 0xd561472c;
		h->x[2] = 0xd6609239;
		h->x[1] = 0x66bd282f;
		h->x[0] = 0xc0c42a98;
		h->y[6] = 0x00000000;
		h->y[5] = 0x040ef170;
		h->y[4] = 0x911cff08;
		h->y[3] = 0x252fae8c;
		h->y[2] = 0xc809ec62;
		h->y[1] = 0x13772d4b;
		h->y[0] = 0x78c6ad16;
	} else {
		if (strcmp(s, "attr2") == 0) {
			h->x[6] = 0x00000000; // v- this is just for the first attribute!
			h->x[5] = 0x03adfc87;
			h->x[4] = 0x32f03c7f;
			h->x[3] = 0x5162de0e;
			h->x[2] = 0x2bcf927c;
			h->x[1] = 0x4b1b8d17;
			h->x[0] = 0x1d542056;
			h->y[6] = 0x00000000;
			h->y[5] = 0x0dad9ea8;
			h->y[4] = 0x0e1294e3;
			h->y[3] = 0x2a54b901;
			h->y[2] = 0xe120d7cf;
			h->y[1] = 0x1c1061ba;
			h->y[0] = 0x3d6c31a5;
		} else {
			if (strcmp(s, "attr3") == 0) {
				h->x[6] = 0x00000000; // v- this is just for the first attribute!
				h->x[5] = 0x1289d47d;
				h->x[4] = 0x128635de;
				h->x[3] = 0xfc1fbbd3;
				h->x[2] = 0x0d5f16b8;
				h->x[1] = 0xad972566;
				h->x[0] = 0xefaec851;
				h->y[6] = 0x00000000;
				h->y[5] = 0x0891b7a1;
				h->y[4] = 0xde45a812;
				h->y[3] = 0xd2b89f46;
				h->y[2] = 0xc97c9962;
				h->y[1] = 0x0fe7aea4;
				h->y[0] = 0x652b3238;
			} 
		}
	}
#endif
#endif
#else	
	// compute y for given (hash) x value  

	ECC_Random_Hash(h, r);
	
#endif
}

#ifdef CPABE_KEYGEN
/**
 * @brief Generate a secret key related to some attributes.
 *
 * @param prv Secret key related to attributes specified
 * @param pub Public key
 * @param msk Master key
 * @param attributes Attributes to include in secret key (should be element wise parsed attributes?)
 */
/*
 daniel@ubuntu:~/cp-abe/cpabe-0.9/bin$ ./cpabe-keygen -d pub_key master_key attr1 attr2 attr3
 r 66 11 d3 ea 4b 3b 88 81 7 6c 71 49 9e 53 45 ae 
 g_r.x 1c aa fc 3f 69 8f d0 b 7 a5 ff 5f 3a 2c 36 45 41 f3 ee 91 3c 56 16 48 
 g_r.y 18 df 57 e8 30 ea 8f 2d 81 72 d8 d2 99 f1 c8 61 a7 f5 7d aa fe c5 7e 5c 
 beta_inv 48 43 b8 25 78 42 24 cf 7c 2c a6 da 85 14 a6 90 
 prv->d.x 1d f1 d1 a3 c9 e3 3a 3a 2f 3a a8 46 69 77 84 3a 44 13 c6 5b c0 91 c1 6c 
 prv->d.y 1 7e c9 79 6c d1 a5 31 34 45 73 cf a9 a4 7f 6f d1 28 57 b4 c3 b3 b6 24 
 Values for attrib: attr1.
 h_rp.x (hash) 13 c4 df 84 a0 81 e6 8c d5 61 47 2c d6 60 92 39 66 bd 28 2f c0 c4 2a 98 
 h_rp.y (hash) 4 e f1 70 91 1c ff 8 25 2f ae 8c c8 9 ec 62 13 77 2d 4b 78 c6 ad 16 
 rp 51 d6 84 b6 92 7b e1 96 9a 9b f5 69 d5 0 7d 62 
 h_rp.x c d4 51 f9 db 3a ed 42 f3 72 46 53 b8 8 22 f0 76 b1 10 1b 5a 2a e4 1a 
 h_rp.y 14 f0 92 c8 c9 49 9e eb ff 81 2d 6d 2e 4d 6d 25 9f a4 16 2a 8a d3 fa 29 
 c.d.x 11 a3 d6 e2 19 b9 22 26 46 88 d6 f2 f4 eb 6a 4 6b 59 c1 93 d6 f6 f2 28 
 c.d.y 17 ff ed 45 d6 1b 6 d2 50 79 11 ea 9d e 53 dd 42 73 16 9 a3 72 e2 f 
 c.dp.x 10 4b f2 f1 ee d8 bf 57 a ec 83 8c b 7c 9 b3 5d 6e 3f ca 3f a2 0 74 
 c.dp.y 6 67 a1 17 88 2a da e2 c 66 35 28 e8 8e 4d d5 98 3 68 d8 bb a2 e 35 
 Values for attrib: attr2.
 h_rp.x (hash) 3 ad fc 87 32 f0 3c 7f 51 62 de e 2b cf 92 7c 4b 1b 8d 17 1d 54 20 56 
 h_rp.y (hash) d ad 9e a8 e 12 94 e3 2a 54 b9 1 e1 20 d7 cf 1c 10 61 ba 3d 6c 31 a5 
 rp 30 9e e2 c0 dc eb 7c f4 3 f3 61 8f 41 da 22 b6 
 h_rp.x 1a 79 e7 30 f2 6b 68 6c 66 83 82 42 bd eb 82 1e 29 29 f3 38 b4 97 79 74 
 h_rp.y 17 65 ac 2c 10 c8 ab 20 b8 43 6c 7d 51 ec 29 65 cc c8 31 a0 d8 5f f4 14 
 c.d.x c 66 e2 e9 ef 2e 72 7a 4b 42 cd 2d 3b 6e 32 a7 44 40 31 7f be de b 78 
 c.d.y 8 6d 7c c3 8b 56 e9 68 c9 e0 a7 4e 32 8 18 43 e2 5d da e2 2a c0 7a d6 
 c.dp.x 9 b2 bf 75 0 f9 af 80 71 67 eb 12 1f 51 c6 66 3c 66 fe 62 98 a5 57 9c 
 c.dp.y 10 a0 60 5b 83 90 10 a9 66 29 e8 d7 65 ff b1 ae 57 6 ce b0 c5 f8 96 0 
 Values for attrib: attr3.
 h_rp.x (hash) 12 89 d4 7d 12 86 35 de fc 1f bb d3 d 5f 16 b8 ad 97 25 66 ef ae c8 51 
 h_rp.y (hash) 8 91 b7 a1 de 45 a8 12 d2 b8 9f 46 c9 7c 99 62 f e7 ae a4 65 2b 32 38 
 rp 4b 9d 41 1e fb 6b 78 5b ba 75 d0 58 7e 57 18 e9 
 h_rp.x c cc 13 15 7e bc b3 27 b7 a9 c8 5b e5 34 34 c9 fc 27 db a5 47 5 71 9f 
 h_rp.y d d9 cf 4a ee b2 6c bd 4c e0 2f 95 4 51 ac 66 98 96 67 cf 74 a5 e2 87 
 c.d.x c 8e f2 d1 46 29 2a 22 7c d8 4f e4 b9 68 de 7b da 74 6d 24 d3 49 53 8c 
 c.d.y 4 e8 d8 30 ca 9d 77 5d a5 42 10 d5 73 71 98 e6 da 2c b7 c3 5c fc 69 3e 
 c.dp.x b 93 81 9a 9b ed 12 72 b4 f0 6f ae d9 e0 85 29 37 5f 64 a8 f5 a3 7f cf 
 c.dp.y 19 19 b1 d7 b1 d8 50 87 c2 73 fa 29 ea 5 ef 30 d5 24 35 59 a5 da 49 d1  
 */
void cpabe_keygen(cpabe_prv_t *prv, cpabe_pub_t *pub, cpabe_msk_t *msk, char** attributes) {
	Point g_r; // G2
	NN_DIGIT r[NUMWORDS]; // Zr
	NN_DIGIT beta_inv[NUMWORDS]; // Zr
	Point tmp;
	cpabe_prv_comp_t *c;
	Point h_rp;
	NN_DIGIT rp[NUMWORDS];
	
	if (prv == NULL || pub == NULL || msk == NULL || attributes == NULL) {
//			printf("cpabe_setup - ERROR: pub or msk NULL! pub: %p, msk: %p", pub, msk);
		return;
	}
	
	// init
	MYLIST(prv->comps);
	list_init(prv->comps);
	
	// compute
#ifdef CPABE_DEBUG
#ifdef SIXTEEN_BIT_PROCESSOR
	r[12] = 0x0000;
	r[11] = 0x0000;
	r[10] = 0x0000;
	r[9] = 0x0000;
	r[8] = 0x0000;
	r[7] = 0x6611;
	r[6] = 0xd3ea;
	r[5] = 0x4b3b;
	r[4] = 0x8881;
	r[3] = 0x076c;
	r[2] = 0x7149;
	r[1] = 0x9e53;
	r[0] = 0x45ae;
#endif
#ifdef THIRTYTWO_BIT_PROCESSOR
#ifdef CPABE192K2
	r[6] = 0x00000000;
	r[5] = 0x00000000;
	r[4] = 0x00000000;
	r[3] = 0x6611d3ea;
	r[2] = 0x4b3b8881;
	r[1] = 0x076c7149;
	r[0] = 0x9e5345ae;
#endif
#endif
#else
	NNModRandom(r, param.m, NUMWORDS);
#endif
	ECC_mul(&g_r, &(pub->gp), r);						/**< g_r = gp * r */
#ifdef CPABE_DEBUG
	debug_print("g_r_x: ", g_r.x);
	debug_print("g_r_y: ", g_r.y);
#endif

	ECC_add(&(prv->d), &(msk->g_alpha), &g_r);			/**< d = g_alpha + g_r; here is P * P -> Add them !!! */
	NNModInv(beta_inv, msk->beta, param.m, NUMWORDS);	/**< beta_inv = 1/beta */

#ifdef CPABE_DEBUG	
	debug_print("beta_inv: ", beta_inv);
#endif
	
	ECC_assign(&tmp, &(prv->d));
	ECC_mul(&(prv->d), &tmp, beta_inv);				/**< d = d * beta_inverted */
	
	// for all attributes generate params
	while( *attributes )
	{
		c = (cpabe_prv_comp_t *) memb_alloc(&prv_comps_m); /**/ memb_comp_count++; /**/

		c->attr = (char *) malloc(strlen(*(attributes)) + 1); /**/ mem_count += strlen(*(attributes)) + 1; /**/
		memcpy(c->attr, *(attributes++), strlen(*(attributes))+1);
		
		point_from_string(&h_rp, c->attr);
#ifdef CPABE_DEBUG
printf("c_attrib: %s\n", c->attr);

#ifdef SIXTEEN_BIT_PROCESSOR
	if (strcmp(c->attr, "attr1") == 0) {
		rp[12] = 0x0000;// v- this is just for the first attribute!
		rp[11] = 0x0000;
		rp[10] = 0x0000;
		rp[9] = 0x0000;
		rp[8] = 0x0000;
		rp[7] = 0x51d6;
		rp[6] = 0x84b6;
		rp[5] = 0x927b;
		rp[4] = 0xe196;
		rp[3] = 0x9a9b;
		rp[2] = 0xf569;
		rp[1] = 0xd500;
		rp[0] = 0x7d62;
	} else {
		if (strcmp(c->attr, "attr2") == 0) {
			rp[12] = 0x0000;
			rp[11] = 0x0000;
			rp[10] = 0x0000;
			rp[9] = 0x0000;
			rp[8] = 0x0000;
			rp[7] = 0x309e;
			rp[6] = 0xe2c0;
			rp[5] = 0xdceb;
			rp[4] = 0x7cf4;
			rp[3] = 0x03f3;
			rp[2] = 0x618f;
			rp[1] = 0x41da;
			rp[0] = 0x22b6;
		} else {
			if (strcmp(c->attr, "attr3") == 0) {
				rp[12] = 0x0000;
				rp[11] = 0x0000;
				rp[10] = 0x0000;
				rp[9] = 0x0000;
				rp[8] = 0x0000;
				rp[7] = 0x4b9d;
				rp[6] = 0x411e;
				rp[5] = 0xfb6b;
				rp[4] = 0x785b;
				rp[3] = 0xba75;
				rp[2] = 0xd058;
				rp[1] = 0x7e57;
				rp[0] = 0x18e9;
			} 
		}
	}
#endif
#ifdef THIRTYTWO_BIT_PROCESSOR
#ifdef CPABE192K2
		if (strcmp(c->attr, "attr1") == 0) {
			rp[6] = 0x00000000;// v- this is just for the first attribute!
			rp[5] = 0x00000000;
			rp[4] = 0x00000000;
			rp[3] = 0x51d684b6;
			rp[2] = 0x927be196;
			rp[1] = 0x9a9bf569;
			rp[0] = 0xd5007d62;
		} else {
			if (strcmp(c->attr, "attr2") == 0) {
				rp[6] = 0x00000000;
				rp[5] = 0x00000000;
				rp[4] = 0x00000000;
				rp[3] = 0x309ee2c0;
				rp[2] = 0xdceb7cf4;
				rp[1] = 0x03f3618f;
				rp[0] = 0x41da22b6;
			} else {
				if (strcmp(c->attr, "attr3") == 0) {
					rp[6] = 0x00000000;
					rp[5] = 0x00000000;
					rp[4] = 0x00000000;
					rp[3] = 0x4b9d411e;
					rp[2] = 0xfb6b785b;
					rp[1] = 0xba75d058;
					rp[0] = 0x7e5718e9;
				} 
			}
		}
#endif
#endif
#else
 		NNModRandom(rp, param.m, NUMWORDS);
#endif
				
		ECC_assign(&tmp, &h_rp);
		ECC_mul(&h_rp, &tmp, rp);					/**< h_rp = h * rp */
			
		ECC_add(&(c->d), &g_r, &h_rp);				/**< d = g_r + h_rp */
		ECC_mul(&(c->dp), &(pub->g), rp);				/**< dp = g * rp */
		
#ifdef CPABE_DEBUG		
		debug_print("CPABE_c_d_x: ", c->d.x);
		debug_print("CPABE_c_d_y: ", c->d.y);
		debug_print("CPABE_c_dp_x: ", c->dp.x);
		debug_print("CPABE_c_dp_y: ", c->dp.y);
#endif		
		// clear h_rb, rp 
		
		list_add(prv->comps, c);
	}
}
#endif

/* ** CP-ABE Encryption ********************* */

#ifdef CPABE_ENCRYPTION
cpabe_policy_t*
base_node( int k, char* s )
{
	cpabe_policy_t* p;
	
	p = (cpabe_policy_t*) memb_alloc(&enc_policy_m); /**/ memb_policy_count++; /**/
	p->k = k;
	if (s != NULL) {
		p->attr = (char *) malloc (strlen(s)+1); /**/ mem_count += strlen(s)+1; /**/
		memcpy(p->attr, s, strlen(s)+1);							// replaced strdup()
	} else {
		p->attr = 0;
	}
	MYLIST(p->children);
	list_init(p->children);
	MYLIST(p->satl);
	list_init(p->satl);
	p->q = 0;
	
	return p;
}

int
parse_policy_postfix( cpabe_cph_t *cph, char* s )
{
	char** toks;
	char** cur_toks;
	char*  tok;
	cpabe_policy_t* node;
	
	toks     = strsplit(s);
	cur_toks = toks;
	
	MYLIST(cph->p);
	list_init(cph->p);
	
	while( *cur_toks )
	{
		uint8_t i, k, n;
		
		tok = *(cur_toks++);
		
		if( !*tok )
			continue;
		
		if( sscanf_repl(tok, /*"%dof%d",*/ &k, &n) != 2 ) {			// sscanf not in library, replaced
		// push leaf token 
			list_push(cph->p, base_node(1, tok));
//printf("parser(%s): k=1 used. (k=%d, n=%d) &cph->p=%p\n", tok, k, n, cph->p);		
		} else {
			// parse "kofn" operator 
//printf("parser(%s): k=%d, n=%d\n", tok, k, n);		
			
			if( k < 1 )
			{
#ifdef CPABE_DEBUG
				printf("error parsing \"%s\": trivially satisfied operator \"%s\"\n", s, tok);
#endif
				return 1;
			}
			else if( k > n )
			{
#ifdef CPABE_DEBUG
				printf("error parsing \"%s\": unsatisfiable operator \"%s\"\n", s, tok);
#endif
				return 1;
			}
			else if( n == 1 )
			{
#ifdef CPABE_DEBUG
				printf("error parsing \"%s\": identity operator \"%s\"\n", s, tok);
#endif
				return 1;
			}
			else if( n > list_length(cph->p) )
			{
#ifdef CPABE_DEBUG
				printf("error parsing \"%s\": stack underflow at \"%s\" (n=%d > list_length=%d)\n", s, tok, n, list_length(cph->p));
#endif
				return 1;
			}
			
			// pop n things and fill in children 
			node = base_node(k, 0);
			for( i = 0; i < n; i++ )
				list_push(node->children, list_pop(cph->p));
			
			// push result 
			list_push(cph->p, node);
		}
	}
	
	if( list_length(cph->p) > 1 )
	{
#ifdef CPABE_DEBUG
		printf("error parsing \"%s\": extra tokens left on stack\n", s);
#endif
		return 1;
	}
	else if( list_length(cph->p) < 1 )
	{
#ifdef CPABE_DEBUG
		printf("error parsing \"%s\": empty policy\n", s);
#endif
		return 1;
	}
		
 	freesplit(toks);
	return 0;
}

cpabe_polynomial_t*
rand_poly( int deg, NN_DIGIT zero_val[NUMWORDS] )
{
	int i;
	cpabe_polynomial_t* q;
	
	q = (cpabe_polynomial_t*) memb_alloc(&enc_polynomial_m); /**/ memb_poly_count++; /**/
	q->deg = deg;
	q->coef = (NN_DIGIT *) malloc(sizeof(NN_DIGIT) * NUMWORDS * (deg + 1));	/**/ mem_count += sizeof(NN_DIGIT) * NUMWORDS * (deg+1); /**/  // gets freed at end of fill_policy
	
	NNAssign(q->coef, zero_val, NUMWORDS);
	
	for( i = 1; i < q->deg + 1; i++ ) {
#ifdef CPABE_DEBUG
#ifdef SIXTEEN_BIT_PROCESSOR
		(q->coef + (i * NUMWORDS))[12] = 0x0000;
		(q->coef + (i * NUMWORDS))[11] = 0x0000;
		(q->coef + (i * NUMWORDS))[10] = 0x0000;
		(q->coef + (i * NUMWORDS))[9] = 0x0000;
		(q->coef + (i * NUMWORDS))[8] = 0x0000;
		(q->coef + (i * NUMWORDS))[7] = 0x4b9d;
		(q->coef + (i * NUMWORDS))[6] = 0x411e;
		(q->coef + (i * NUMWORDS))[5] = 0xfb6b;
		(q->coef + (i * NUMWORDS))[4] = 0x785b;
		(q->coef + (i * NUMWORDS))[3] = 0xba75;
		(q->coef + (i * NUMWORDS))[2] = 0xd058;
		(q->coef + (i * NUMWORDS))[1] = 0x7e57;
		(q->coef + (i * NUMWORDS))[0] = 0x18e9;
#endif
#ifdef THIRTYTWO_BIT_PROCESSOR
#ifdef CPABE192K2
		(q->coef + (i * NUMWORDS))[6] = 0x00000000;
		(q->coef + (i * NUMWORDS))[5] = 0x00000000;
		(q->coef + (i * NUMWORDS))[4] = 0x00000000;
		(q->coef + (i * NUMWORDS))[3] = 0x4b9d411e;
		(q->coef + (i * NUMWORDS))[2] = 0xfb6b785b;
		(q->coef + (i * NUMWORDS))[1] = 0xba75d058;
		(q->coef + (i * NUMWORDS))[0] = 0x7e5718e9;
#endif
#endif
#else
 		NNModRandom(q->coef + (i * NUMWORDS), param.m, NUMWORDS);			// Array addressing correct?
#endif
	}
		
	return q;
}

void
eval_poly( NN_DIGIT r[NUMWORDS], cpabe_polynomial_t* q, NN_DIGIT x[NUMWORDS] )
{
	int j;
	NN_DIGIT tmp[2*NUMWORDS]; // for mult op
	NN_DIGIT s[NUMWORDS]; // Zr
	NN_DIGIT t[NUMWORDS]; // Zr
		
	NNAssignZero(r, NUMWORDS);
	NNAssignOne(t, NUMWORDS);
	
	for( j = 0; j < q->deg + 1; j++ )
	{
		watchdog_periodic();
		// r += q->coef[i] * t 
		NNMult(tmp, q->coef + (j * NUMWORDS), t, NUMWORDS);		// more efficent than NNModMult, because of m!	
//		NNMod(tmp, tmp, 2*NUMWORDS, param.m, NNDigits(param.m, NUMWORDS));
		NNDiv(NULL, tmp, tmp, 2*NUMWORDS, param.m, NNDigits(param.m, NUMWORDS));
		NNAssignZero(s, NUMWORDS);
		NNAssign(s, tmp, NNDigits(param.m, NUMWORDS));
//		NNModMultVar(s, q->coef + (j * NUMWORDS), t, NUMWORDS, param.m, NNDigits(param.m, NUMWORDS)); // FIXME: should replace above
		NNModAdd(r, r, s, param.m, NUMWORDS);
		
		// t *= x 
		NNModMult(t, t, x, param.m, NUMWORDS);
		
#ifdef CPABE_DEBUG		
		printf("eval_poly: j=%d, deg=%d\n", j, q->deg);
		
		debug_print("s: ", s);
		debug_print("r: ", r);
		debug_print("t: ", t);
		debug_print("x: ", x);
#endif		

	}
}

void
fill_policy(cpabe_policy_t *p, cpabe_pub_t *pub, NN_DIGIT e[NUMWORDS]) {
	cpabe_policy_t* cp;
	int i;
	NN_DIGIT r[NUMWORDS]; // Zr
	NN_DIGIT t[NUMWORDS]; // Zr
	Point h;			  // G2
		
#ifdef CPABE_DEBUG		
	printf("p_attrib: %s\n", p->attr);
#endif
	p->q = rand_poly(p->k - 1, e);
	
	if( list_length(p->children) == 0 )
	{		
		point_from_string(&h, p->attr);
		ECC_mul(&(p->c),  &(pub->g), &(p->q->coef[0]));
		ECC_mul(&(p->cp), &h,     &(p->q->coef[0]));

#ifdef CPABE_DEBUG		
		debug_print("CPABE_h_x: ", h.x);
		debug_print("CPABE_h_y: ", h.y);
		debug_print("CPABE_p_c_x: ", p->c.x);
		debug_print("CPABE_p_c_y: ", p->c.y);
		debug_print("CPABE_p_cp_x: ", p->cp.x);
		debug_print("CPABE_p_cp_y: ", p->cp.y);
#endif		
	} else {
		i = 1;
		for(cp = list_head(p->children); cp != NULL; cp = cp->next )
			{
				NNAssignDigit(r, i, NUMWORDS);				/**< Assign i+1 to r */
				eval_poly(t, p->q, r);
				fill_policy(cp, pub, t);				/**< ...but supply element i */
				
				watchdog_periodic();
				i++;
			}
	}
	
	// everything is done and I can free q
	/**/ mem_free_count += sizeof(NN_DIGIT) * NUMWORDS * (p->q->deg + 1); /**/
	free(p->q->coef); 
	memb_free(&enc_polynomial_m, p->q); /**/ memb_poly_free_count++; /**/
}

/**
 * @brief generate symetric encryption key m with public key and a specified policy.
 */
/*
policy: attr1 attr3 2of2

m 1b f4 66 fb af 33 f4 0 3a 92 20 2d bf 61 56 bb 87 92 de 92 e9 1a b3 a7 7 35 f4 6d 77 eb 1 8f 5a 72 41 47 db cf 9f 9e f9 c5 c0 8c c5 77 f9 fc 
s 30 9e e2 c0 dc eb 7c f4 3 f3 61 8f 41 da 22 b6 
cph->cs 3 e7 16 18 f7 6e be 30 8b 3b 27 24 aa f0 6e c7 2a 5d 59 74 18 ce 8e 2f 1d e2 60 7f 96 1a ff 4c c1 b3 f3 f7 b 74 82 34 0 d1 82 d9 44 dd ee 3f 
cph->c.x 12 27 3a 28 85 bc 33 5c 67 6e c1 5a b9 53 96 1e 87 55 14 74 ac cc 22 93 
cph->c.y 1d f3 70 6c 3a db 14 ac 50 43 61 bf 27 3b 75 de 9f 28 9a 42 16 d2 13 d1 

policy node: (null)
q->coef[1] 4b 9d 41 1e fb 6b 78 5b ba 75 d0 58 7e 57 18 e9 
eval_poly: j=0, deg=1
s 30 9e e2 c0 dc eb 7c f4 3 f3 61 8f 41 da 22 b6 
r 30 9e e2 c0 dc eb 7c f4 3 f3 61 8f 41 da 22 b6 
t 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
x 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
eval_poly: j=1, deg=1
s 4b 9d 41 1e fb 6b 78 5b ba 75 d0 58 7e 57 18 e9 
r 7c 3c 23 df d8 56 f5 4f be 69 31 e7 c0 31 3b 9f 
t 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
x 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 

policy node: attr1
h.x 13 c4 df 84 a0 81 e6 8c d5 61 47 2c d6 60 92 39 66 bd 28 2f c0 c4 2a 98 
h.y 4 e f1 70 91 1c ff 8 25 2f ae 8c c8 9 ec 62 13 77 2d 4b 78 c6 ad 16 
p->c.x a 83 8c 17 96 fa 71 fa 2e 7 52 2b 90 bf 89 a2 19 1a 8e ca 3e 7b 73 11 
p->c.y 1d ac a 87 84 fc 24 69 f1 aa 68 7c ad 98 8e 7a 41 27 9f 42 4f 3d 6f 9b 
p->cp.x 11 4 1d d 52 96 ac d6 2e 9a b5 b8 de 4b 8f ea 94 b2 af 6 a9 f4 4f 53 
p->cp.y 9 a4 f2 f e6 c a5 dc ec 4e 4f 93 d2 5d c5 da db 34 24 6a 1 da 3b 56 
eval_poly: j=0, deg=1
s 30 9e e2 c0 dc eb 7c f4 3 f3 61 8f 41 da 22 b6 
r 30 9e e2 c0 dc eb 7c f4 3 f3 61 8f 41 da 22 b6 
t 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 
x 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 
eval_poly: j=1, deg=1
s 17 3a 81 fd f6 d6 f0 b7 74 eb a0 b0 fc ae 31 d1 
r 47 d9 64 be d3 c2 6d ab 78 df 2 40 3e 88 54 87 
t 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 4 
x 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 

policy node: attr3
h.x 12 89 d4 7d 12 86 35 de fc 1f bb d3 d 5f 16 b8 ad 97 25 66 ef ae c8 51 
h.y 8 91 b7 a1 de 45 a8 12 d2 b8 9f 46 c9 7c 99 62 f e7 ae a4 65 2b 32 38 
p->c.x 10 a5 dc 80 45 f 18 30 41 8e 63 90 c0 27 e2 a4 8a aa e b5 40 60 27 2b 
p->c.y d b8 76 d0 d8 68 ff d 29 c3 d7 7 68 d7 56 97 ed af a2 e3 a4 c1 c9 3d 
p->cp.x 2 d6 ac 21 17 76 19 78 8c 56 a7 af 43 fd 18 6d 21 d8 6d 5c f8 de 9 72 
p->cp.y e 91 f4 e2 57 af 15 92 f1 f4 7c d0 be 56 80 f9 43 70 25 de 4c 94 47 2b  */
void cpabe_enc(cpabe_cph_t *cph, cpabe_pub_t *pub, NN2_NUMBER * m, char *policy) {
 	NN_DIGIT s[NUMWORDS]; // Zr
	
	// initialize 
	parse_policy_postfix(cph, policy);
	
	// compute 
#ifdef CPABE_DEBUG
#ifdef SIXTEEN_BIT_PROCESSOR
	m->r[12] = 0x0000;
	m->r[11] = 0x1bf4;
	m->r[10] = 0x66fb;
	m->r[9] = 0xaf33;
	m->r[8] = 0xf400;
	m->r[7] = 0x3a92;
	m->r[6] = 0x202d;
	m->r[5] = 0xbf61;
	m->r[4] = 0x56bb;
	m->r[3] = 0x8792;
	m->r[2] = 0xde92;
	m->r[1] = 0xe91a;
	m->r[0] = 0xb3a7;
	m->i[12] = 0x0000;
	m->i[11] = 0x0735;
	m->i[10] = 0xf46d;
	m->i[9] = 0x77eb;
	m->i[8] = 0x018f;
	m->i[7] = 0x5a72;
	m->i[6] = 0x4147;
	m->i[5] = 0xdbcf;
	m->i[4] = 0x9f9e;
	m->i[3] = 0xf9c5;
	m->i[2] = 0xc08c;
	m->i[1] = 0xc577;
	m->i[0] = 0xf9fc;

	s[12] = 0x0000;
	s[11] = 0x0000;
	s[10] = 0x0000;
	s[9] = 0x0000;
	s[8] = 0x0000;
	s[7] = 0x309e;
	s[6] = 0xe2c0;
	s[5] = 0xdceb;
	s[4] = 0x7cf4;
	s[3] = 0x03f3;
	s[2] = 0x618f;
	s[1] = 0x41da;
	s[0] = 0x22b6;
#endif
#ifdef THIRTYTWO_BIT_PROCESSOR
#ifdef CPABE192K2
	m->r[6] = 0x00000000;
	m->r[5] = 0x1bf466fb;
	m->r[4] = 0xaf33f400;
	m->r[3] = 0x3a92202d;
	m->r[2] = 0xbf6156bb;
	m->r[1] = 0x8792de92;
	m->r[0] = 0xe91ab3a7;
	m->i[6] = 0x00000000;
	m->i[5] = 0x0735f46d;
	m->i[4] = 0x77eb018f;
	m->i[3] = 0x5a724147;
	m->i[2] = 0xdbcf9f9e;
	m->i[1] = 0xf9c5c08c;
	m->i[0] = 0xc577f9fc;
	
	s[6] = 0x00000000;
	s[5] = 0x00000000;
	s[4] = 0x00000000;
	s[3] = 0x309ee2c0;
	s[2] = 0xdceb7cf4;
	s[1] = 0x03f3618f;
	s[0] = 0x41da22b6;
#endif
#endif
#else
 	NN2ModRandom(m, param.p, NUMWORDS);
 	NNModRandom(s, param.m, NUMWORDS);
#endif
	NN2ModExp(&(cph->cs),  &(pub->g_hat_alpha), s, param.p, NUMWORDS);	
	NN2ModMult(&(cph->cs), &(cph->cs), m, param.p, NUMWORDS);		/**< cs = e(g, g)^(alpha * s) * m */
	
	ECC_mul(&(cph->c), &(pub->h), s);						/**< c = h ^ s */
	
	fill_policy((cpabe_policy_t *) list_head(cph->p), pub, s);
}
#endif

/* ** CP-ABE Decryption ********************* */

#ifdef CPABE_DECRYPTION
void
check_sat( cpabe_policy_t * p, cpabe_prv_t * prv )
{
	int i, l;
	
	p->satisfiable = 0;
	if( list_length(p->children) == 0 )
	{
		for( i = 0; i < list_length(prv->comps); i++ ) {
			if( !strcmp(((cpabe_prv_comp_t *) list_index(prv->comps, i))->attr,	
						p->attr) )
			{
				p->satisfiable = 1;
				p->attri = i;
				break;
			}
		}
	}
	else
	{
		for( i = 0; i < list_length(p->children); i++ )
			check_sat(list_index(p->children, i), prv);
		
		l = 0;
		for( i = 0; i < list_length(p->children); i++ )
			if( ((cpabe_policy_t*) list_index(p->children, i))->satisfiable )
				l++;
		
		if( l >= p->k )
			p->satisfiable = 1;
	}
}


/*
void
pick_sat_naive( cpabe_policy_t* p, cpabe_prv_t* prv )
{
	int i, k, l;
	satl_int_t * alloc_k;
	
//	assert(p->satisfiable == 1);
	
	if( list_length(p->children) == 0 )
		return;
	
	MYLIST(p->satl);
	list_init(p->satl);
	
	l = 0;
	for( i = 0; i < list_length(p->children) && l < p->k; i++ )
		if( ((cpabe_policy_t*) list_index(p->children, i))->satisfiable )
		{
			pick_sat_naive(list_index(p->children, i), prv);
			l++;
			k = i + 1;
			alloc_k = (satl_int_t *) malloc(sizeof(satl_int_t));
			alloc_k->k = k;
			list_add(p->satl, alloc_k);
		}
}
*/


int
cmp_int( const void* a, const void* b )
{
	int k, l;
	
	k = ((cpabe_policy_t*) list_index(cur_comp_pol->children, *((int*)a)))->min_leaves;
	l = ((cpabe_policy_t*) list_index(cur_comp_pol->children, *((int*)b)))->min_leaves;
	
	return
	k <  l ? -1 :
	k == l ?  0 : 1;
}

void
pick_sat_min_leaves( cpabe_policy_t* p, cpabe_prv_t* prv )
{
	int i, k, l;
	int* c;
	satl_int_t * alloc_k;
	
//	assert(p->satisfiable == 1);
#ifdef CPABE_DEBUG
	printf("p->attr=%s\n", p->attr);
#endif
	
	if( list_length(p->children) == 0 )
		p->min_leaves = 1;
	else
	{
		for( i = 0; i < list_length(p->children); i++ )
			if( ((cpabe_policy_t*) list_index(p->children, i))->satisfiable )
				pick_sat_min_leaves(list_index(p->children, i), prv);

		c = malloc(sizeof(int) * list_length(p->children));	/**/ mem_count += sizeof(int) * list_length(p->children); /**/
		for( i = 0; i < list_length(p->children); i++ )
			c[i] = i;
		
		cur_comp_pol = p;
		qsort(c, list_length(p->children), sizeof(int), cmp_int);
		
//		MYLIST(p->satl);	// this was moved to ease freeing memory
//		list_init(p->satl);
		p->min_leaves = 0;
		l = 0;
		
		for( i = 0; i < list_length(p->children) && l < p->k; i++ )
			if( ((cpabe_policy_t*) list_index(p->children, c[i]))->satisfiable )
			{
				l++;
				p->min_leaves += ((cpabe_policy_t*) list_index(p->children, c[i]))->min_leaves;
				k = c[i] + 1;
//				g_array_append_val(p->satl, k);
				alloc_k = (satl_int_t *) malloc(sizeof(satl_int_t)); /**/ mem_count += sizeof(satl_int_t); /**/
				alloc_k->k = k;
				list_add(p->satl, alloc_k);

#ifdef CPABE_DEBUG
	printf("p->satl[%d]->k=%d\n", i, k);
#endif
			}
//		assert(l == p->k);
		
		/**/ mem_free_count += sizeof(int) * list_length(p->children); /**/
		free(c);
	}
}



void
lagrange_coef( NN_DIGIT r[NUMWORDS], list_t s, int i )
{
	int j, k;
	NN_DIGIT tmp1[NUMWORDS]; // Zr
	NN_DIGIT tmp2[NUMWORDS]; // Zr
	NN_DIGIT t[NUMWORDS];	// Zr
	NN_DIGIT r_tmp[2*NUMWORDS];
			
	NNAssignOne(r, NUMWORDS);
	for( k = 0; k < list_length(s); k++ )
	{
		j = ((satl_int_t *) list_index(s, k))->k;
		if( j == i )
			continue;

		NNAssignDigit(tmp1, j, NUMWORDS);		
		NNModNeg(t, tmp1, param.m, NUMWORDS);							// t = -j

//		NNModMult(r, r, t, param.m, NUMWORDS); /* num_muls++; */
		NNMult(r_tmp, r, t, NUMWORDS);		// faster than NNModMult, because of small m!	
		NNDiv(NULL, r_tmp, r_tmp, 2*NUMWORDS, param.m, NNDigits(param.m, NUMWORDS));
		NNAssignZero(r, NUMWORDS);
		NNAssign(r, r_tmp, NNDigits(param.m, NUMWORDS));
#ifdef CPABE_DEBUG		
		printf("lag_t1[%d]: ", k);
		debug_print("", t);
		printf("lag_r1[%d]: ", k);
		debug_print("",r);
#endif		

		NNAssignDigit(tmp1, j, NUMWORDS);		
		NNAssignDigit(tmp2, i, NUMWORDS);		
		NNModSub(t, tmp2, tmp1, param.m, NUMWORDS);						// t = i - j

		NNModInv(t, t, param.m, NUMWORDS);
//		NNModMult(r, r, t, param.m, NUMWORDS); /* num_muls++; */
		NNMult(r_tmp, r, t, NUMWORDS);		// faster than NNModMult, because of small m!	
		NNDiv(NULL, r_tmp, r_tmp, 2*NUMWORDS, param.m, NNDigits(param.m, NUMWORDS));
		NNAssignZero(r, NUMWORDS);
		NNAssign(r, r_tmp, NNDigits(param.m, NUMWORDS));

#ifdef CPABE_DEBUG		
		printf("lag_t2[%d]: ", k);
		debug_print("", t);
		printf("lag_r2[%d]: ", k);
		debug_print("",r);
#endif		
	}

#ifdef CPABE_DEBUG		
		debug_print("lag_r: ", r);
#endif		
	
	//element_clear(t);
}


/*
#error Naive decryption code NOT ported, yet.
void
dec_leaf_naive( element_t r, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
 {
 bswabe_prv_comp_t* c;
 element_t s;
 
 c = &(g_array_index(prv->comps, bswabe_prv_comp_t, p->attri));
 
 element_init_GT(s, pub->p);
 
 pairing_apply(r, p->c,  c->d,  pub->p); / * num_pairings++; * /
pairing_apply(s, p->cp, c->dp, pub->p); / * num_pairings++; * /
element_invert(s, s);
element_mul(r, r, s); / * num_muls++; * /

element_clear(s);
}

void dec_node_naive( element_t r, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub );

void
dec_internal_naive( element_t r, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
{
	int i;
	element_t s;
	element_t t;
	
	element_init_GT(s, pub->p);
	element_init_Zr(t, pub->p);
	
	element_set1(r);
	for( i = 0; i < p->satl->len; i++ )
	{
		dec_node_naive
		(s, g_ptr_array_index
		 (p->children, g_array_index(p->satl, int, i) - 1), prv, pub);
 		lagrange_coef(t, p->satl, g_array_index(p->satl, int, i));
		element_pow_zn(s, s, t); / * num_exps++; * /
		element_mul(r, r, s); / * num_muls++; * /
	}
	
	element_clear(s);
	element_clear(t);
}

void
dec_node_naive( element_t r, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
{
	assert(p->satisfiable);
	if( p->children->len == 0 )
		dec_leaf_naive(r, p, prv, pub);
	else
		dec_internal_naive(r, p, prv, pub);
}

void
dec_naive( element_t r, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
{
	dec_node_naive(r, p, prv, pub);
}
*/

/*
#error Merge decryption code NOT ported, yet.
void
dec_leaf_merge( element_t exp, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
{
	bswabe_prv_comp_t* c;
	element_t s;
	
	c = &(g_array_index(prv->comps, bswabe_prv_comp_t, p->attri));
	
	if( !c->used )
	{
		c->used = 1;
		element_init_G1(c->z,  pub->p);
		element_init_G1(c->zp, pub->p);
		element_set1(c->z);
		element_set1(c->zp);
	}
	
	element_init_G1(s, pub->p);
	
	element_pow_zn(s, p->c, exp); / * num_exps++; * /
	element_mul(c->z, c->z, s); / * num_muls++; * /
	
	element_pow_zn(s, p->cp, exp); / * num_exps++; * /
	element_mul(c->zp, c->zp, s); / * num_muls++; * /
	
	element_clear(s);
}

void dec_node_merge( element_t exp, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub );

void
dec_internal_merge( element_t exp, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
{
	int i;
	element_t t;
	element_t expnew;
	
	element_init_Zr(t, pub->p);
	element_init_Zr(expnew, pub->p);
	
	for( i = 0; i < p->satl->len; i++ )
	{
 		lagrange_coef(t, p->satl, g_array_index(p->satl, int, i));
		element_mul(expnew, exp, t); / * num_muls++; * /
		dec_node_merge(expnew, g_ptr_array_index
					   (p->children, g_array_index(p->satl, int, i) - 1), prv, pub);
	}
	
	element_clear(t);
	element_clear(expnew);
}

void
dec_node_merge( element_t exp, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
{
	assert(p->satisfiable);
	if( p->children->len == 0 )
		dec_leaf_merge(exp, p, prv, pub);
	else
		dec_internal_merge(exp, p, prv, pub);
}

void
dec_merge( element_t r, bswabe_policy_t* p, bswabe_prv_t* prv, bswabe_pub_t* pub )
{
	int i;
	element_t one;
	element_t s;
	
	/ * first mark all attributes as unused * /
	for( i = 0; i < prv->comps->len; i++ )
		g_array_index(prv->comps, bswabe_prv_comp_t, i).used = 0;
	
	/ * now fill in the z's and zp's * /
	element_init_Zr(one, pub->p);
	element_set1(one);
	dec_node_merge(one, p, prv, pub);
	element_clear(one);
	
	/ * now do all the pairings and multiply everything together * /
	element_set1(r);
	element_init_GT(s, pub->p);
	for( i = 0; i < prv->comps->len; i++ )
		if( g_array_index(prv->comps, bswabe_prv_comp_t, i).used )
		{
			bswabe_prv_comp_t* c = &(g_array_index(prv->comps, bswabe_prv_comp_t, i));
			
			pairing_apply(s, c->z, c->d, pub->p); / * num_pairings++; * /
			element_mul(r, r, s); / * num_muls++; * /
			
			pairing_apply(s, c->zp, c->dp, pub->p); / * num_pairings++; * /
			element_invert(s, s);
			element_mul(r, r, s); / * num_muls++; * /
		}
	element_clear(s);
}
*/


static void
dec_leaf_flatten( NN2_NUMBER * r, NN_DIGIT * exp,
				 cpabe_policy_t* p, cpabe_prv_t* prv )
{
	cpabe_prv_comp_t* c;
	NN2_NUMBER s; // GT
	NN2_NUMBER t; // GT
/*
debug_print("leaf_r_r: ", r->r);
debug_print("leaf_r_i: ", r->i);
debug_print("leaf_exp: ", exp);
	
debug_print("leaf_p-c_x: ", p->c.x);
debug_print("leaf_p-c_y: ", p->c.y);
debug_print("leaf_p-cp_x: ", p->cp.x);
debug_print("leaf_p-cp_y: ", p->cp.y);
*/
	c = (cpabe_prv_comp_t *) list_index(prv->comps, p->attri);
/*	
printf("p->attri: %d - c:%p prv-comp %p\n", p->attri, c, prv->comps);
debug_print("leaf_c-d_x: ", c->d.x);
debug_print("leaf_c-d_y: ", c->d.y);
debug_print("leaf_c-dp_x: ", c->dp.x);
debug_print("leaf_c-dp_y: ", c->dp.y);
*/	
	TP_TatePairing(&s, &(p->c),  &(c->d)); /* num_pairings++; */
	TP_TatePairing(&t, &(p->cp), &(c->dp)); /* num_pairings++; */

#ifdef CPABE_DEBUG	
		printf("p->attr=%s\n", p->attr);
	
		debug_print("leaf_tp_t_r: ", t.r);
		debug_print("leaf_tp_t_i: ", t.i);
		debug_print("leaf_tp_s_r: ", s.r);
		debug_print("leaf_tp_s_i: ", s.i);
#endif		

	NN2ModInv(&t, &t, param.p, NUMWORDS);
	NN2ModMult(&s, &s, &t, param.p, NUMWORDS); /* num_muls++; */
	NN2ModExp(&s, &s, exp, param.p, NUMWORDS); /* num_exps++; */

#ifdef CPABE_DEBUG		
		debug_print("leaf_t_r: ", t.r);
		debug_print("leaf_t_i: ", t.i);
		debug_print("leaf_s_r: ", s.r);
		debug_print("leaf_s_i: ", s.i);
#endif		
	
	NN2ModMult(r, r, &s, param.p, NUMWORDS); /* num_muls++; */

#ifdef CPABE_DEBUG		
		debug_print("leaf_r_r: ",r->r);
		debug_print("leaf_r_i: ",r->i);
#endif		
	
	//element_clear(s);		
	//element_clear(t);
}

static void dec_node_flatten( NN2_NUMBER * r, NN_DIGIT * exp,
					  cpabe_policy_t* p, cpabe_prv_t* prv );

static void
dec_internal_flatten( NN2_NUMBER * r, NN_DIGIT * exp,
					 cpabe_policy_t* p, cpabe_prv_t* prv ) 
{
	int i;
	NN_DIGIT t[NUMWORDS];		// Zr
	NN_DIGIT expnew[NUMWORDS];	// Zr
	NN_DIGIT tmp[2*NUMWORDS];
	
	for( i = 0; i < list_length(p->satl); i++ )
	{
 		lagrange_coef(t, p->satl, ((satl_int_t *) list_index(p->satl, i))->k);
//		NNModMult(expnew, exp, t, param.m, NUMWORDS); /* num_muls++; */
		NNMult(tmp, exp, t, NUMWORDS);		// more efficent than NNModMult, because of small m!	
		NNDiv(NULL, tmp, tmp, 2*NUMWORDS, param.m, NNDigits(param.m, NUMWORDS));
		NNAssignZero(expnew, NUMWORDS);
		NNAssign(expnew, tmp, NNDigits(param.m, NUMWORDS));
		dec_node_flatten(r, expnew, list_index
						 (p->children, ((satl_int_t *) list_index(p->satl, i))->k - 1), prv);

#ifdef CPABE_DEBUG		
		printf("int_r_r[%d]: ", i);
		debug_print("",r->r);
		printf("int_r_i[%d]: ", i);
		debug_print("",r->i);
#endif		
	}
	
	//element_clear(t);
	//element_clear(expnew);
}

static void
dec_node_flatten( NN2_NUMBER * r, NN_DIGIT * exp,
				 cpabe_policy_t* p, cpabe_prv_t* prv )
{
//	assert(p->satisfiable);
	if( list_length(p->children) == 0 )
		dec_leaf_flatten(r, exp, p, prv);
	else
		dec_internal_flatten(r, exp, p, prv);
}

static void
dec_flatten( NN2_NUMBER * r, cpabe_policy_t* p, cpabe_prv_t* prv )
{
	NN_DIGIT one[NUMWORDS];	// Zr
		
	NNAssignOne(one, NUMWORDS);
	NN2AssignNN(r, one, NUMWORDS);
	
	dec_node_flatten(r, one, p, prv);
	
	//element_clear(one);
}


/*
p->attr=(null)
p->attr=attr1
p->attr=attr3
p->satl[0]=1
p->satl[1]=2
lag_t1[1] 80 0 0 3f ff ff ff ff ff ff ff ff ff ff ff ff 
lag_r1[1] 80 0 0 3f ff ff ff ff ff ff ff ff ff ff ff ff 
lag_t2[1] 80 0 0 40 0 0 0 0 0 0 0 0 0 0 0 0 
lag_r2[1] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 
lag_r 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 
p->attr=attr1
leaf_tp_t 17 77 27 43 df 87 56 2f 4c fe 15 64 2c 73 42 20 4d ee 96 9e 95 83 7f f5 8 68 eb 43 88 aa 30 37 28 d9 b8 db d3 22 94 e ff bb 7e d0 18 df 55 94 
leaf_tp_s 16 1 4a 8e 79 c0 bd 54 68 87 b aa 7d 82 96 79 ee 4 e c8 9a 9b 1 c9 15 a8 7e 5d 57 a 44 94 2b 3f b7 4b b1 4e 9a 74 ed 50 7a de b7 11 75 ee 
leaf_t 17 77 27 43 df 87 56 2f 4c fe 15 64 2c 73 42 20 4d ee 96 9e 95 83 7f f5 17 d5 ac 1b f9 eb 41 30 7c b 22 24 2c dd 6b f1 40 c1 af ce ad b4 3d d7 
leaf_s 15 af 2c d2 b5 94 32 54 c6 44 bd eb b8 40 72 b c9 d3 5e 49 35 da 6a c9 1 58 c0 c2 5a 16 8 cb f8 65 8e 31 ee 28 84 20 d5 57 62 b8 74 29 55 b9 
leaf_r 15 af 2c d2 b5 94 32 54 c6 44 bd eb b8 40 72 b c9 d3 5e 49 35 da 6a c9 1 58 c0 c2 5a 16 8 cb f8 65 8e 31 ee 28 84 20 d5 57 62 b8 74 29 55 b9 
int_r[0] 15 af 2c d2 b5 94 32 54 c6 44 bd eb b8 40 72 b c9 d3 5e 49 35 da 6a c9 1 58 c0 c2 5a 16 8 cb f8 65 8e 31 ee 28 84 20 d5 57 62 b8 74 29 55 b9 
lag_t1[0] 80 0 0 40 0 0 0 0 0 0 0 0 0 0 0 0 
lag_r1[0] 80 0 0 40 0 0 0 0 0 0 0 0 0 0 0 0 
lag_t2[0] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
lag_r2[0] 80 0 0 40 0 0 0 0 0 0 0 0 0 0 0 0 
lag_r 80 0 0 40 0 0 0 0 0 0 0 0 0 0 0 0 
p->attr=attr3
leaf_tp_t 1a 30 38 53 c8 e7 40 ac 32 6f 9b cc da 5d e0 c0 e 57 b5 3f 7f fe 1a f2 a 6b d5 55 87 e0 ca 6c b0 e 46 b b7 1f db d6 da ac 3e ac 99 e6 11 39 
leaf_tp_s 1 63 8b f6 be 8 5d 53 e6 80 ed 23 6c a5 fd 11 ae 3 cd 36 96 62 3c 18 d 30 60 2b e1 8 bd cb d0 fb ac 86 38 81 ab 9d 4e 8c 21 e 6d 94 87 57 
leaf_t 1a 30 38 53 c8 e7 40 ac 32 6f 9b cc da 5d e0 c0 e 57 b5 3f 7f fe 1a f2 15 d2 c2 9 fa b4 a6 fa f4 d6 94 f4 48 e0 24 29 65 d0 ef f2 2c ad 82 32 
leaf_s 5 d5 d5 ea a4 8b 83 b 17 3a 8c 89 60 34 7c 39 ad 43 b7 b6 1d a5 71 1a 1c b2 96 96 4a b3 7d e2 ec 10 e0 f 27 3d b0 c2 5 55 9d c5 13 2 6c 88 
leaf_r 3 e5 24 76 8c 30 a9 40 b1 3 63 f0 cc 6d 16 57 5e c8 a8 6b e1 18 d9 e1 0 d6 7e c8 cf be 86 b1 55 e4 a4 fc 4 21 f8 67 4c 78 de 85 e8 5d 7e 30 
int_r[1] 3 e5 24 76 8c 30 a9 40 b1 3 63 f0 cc 6d 16 57 5e c8 a8 6b e1 18 d9 e1 0 d6 7e c8 cf be 86 b1 55 e4 a4 fc 4 21 f8 67 4c 78 de 85 e8 5d 7e 30 
dec_flat_t 3 e5 24 76 8c 30 a9 40 b1 3 63 f0 cc 6d 16 57 5e c8 a8 6b e1 18 d9 e1 0 d6 7e c8 cf be 86 b1 55 e4 a4 fc 4 21 f8 67 4c 78 de 85 e8 5d 7e 30 
dec_t 3 c5 82 15 89 55 47 af a8 d7 68 ad 84 13 bc 32 1a 7 d8 8d c9 2 27 d4 1a e7 4b cb b4 69 2 e7 f8 40 7d 3f da a9 49 90 1d 4c 39 80 b4 18 cb 37 
dec_m 1b f4 66 fb af 33 f4 0 3a 92 20 2d bf 61 56 bb 87 92 de 92 e9 1a b3 a7 7 35 f4 6d 77 eb 1 8f 5a 72 41 47 db cf 9f 9e f9 c5 c0 8c c5 77 f9 fc 
 */
int cpabe_dec(cpabe_prv_t *prv, cpabe_cph_t *cph, NN2_NUMBER * m) {
	NN2_NUMBER t;	// GT
	
	check_sat(list_head(cph->p), prv);							// check properties saturation
	if( !((struct cpabe_policy_s *) list_head(cph->p))->satisfiable )
	{
#ifdef CPABE_DEBUG
		printf("cannot decrypt, attributes in key do not satisfy policy\n");
#endif
		return 0;
	}
//	if( no_opt_sat ) 
//		pick_sat_naive(list_head(cph.p), &prv); 
//	else 
		pick_sat_min_leaves(list_head(cph->p), prv);				
	
//	if( dec_strategy == DEC_NAIVE ) 
//		dec_naive(t, cph->p, prv, pub); // not ported!
//	else if( dec_strategy == DEC_FLATTEN ) 
		dec_flatten(&t, list_head(cph->p), prv);
//	else 
//		dec_merge(t, cph->p, prv, pub); // not ported!

#ifdef CPABE_DEBUG
		debug_print("dec_flat_t_r: ",t.r);
		debug_print("dec_flat_t_i: ",t.i);
#endif
	
	NN2ModMult(m, &(cph->cs), &t, param.p, NUMWORDS); /* num_muls++; */
	
	TP_TatePairing(&t, &(cph->c), &(prv->d)); /* num_pairings++; */
	NN2ModInv(&t, &t, param.p, NUMWORDS);
	NN2ModMult(m, m, &t, param.p, NUMWORDS); /* num_muls++; */ 

#ifdef CPABE_DEBUG
		debug_print("dec_t_r: ",t.r);
		debug_print("dec_t_i: ",t.i);
#endif

	return 1;
}

#endif

/* *** CP-ABE key update / revocation *************************************** */

void cpabe_revocation_update(NN2_NUMBER *g_hat_alpha_prime, Point *delta, cpabe_pub_t *pub, cpabe_msk_t *msk) {
	NN_DIGIT alpha_prime[NUMWORDS];
	NN_DIGIT one_neg[NUMWORDS];
	NN_DIGIT beta_inv[NUMWORDS];
	Point g_alpha_neg;
	Point g_alpha_prime;
	Point tmp;
	
	// g_alpha_prime
	NNModRandom(alpha_prime, param.m, NUMWORDS);		/**< Random alpha' */
	ECC_mul(&(g_alpha_prime), &(pub->gp), alpha_prime);	/**< g_alpha' = gp * alpha' */
	
	// g_hat_alpha_prime
	TP_TatePairing(g_hat_alpha_prime, &(pub->g), &(g_alpha_prime));	/**< g_hat_alpha' = e(g, gp * alpha') */
	
	// delta
	NNAssignOne(one_neg, NUMWORDS);
	NNModNeg(one_neg, one_neg, param.m, NUMWORDS);		/**< one_neg = -1 */
	ECC_mul(&g_alpha_neg, &(msk->g_alpha), one_neg);	/**< g_alpha_neg = g_alpha * -1 */
	ECC_add(delta, &g_alpha_prime, &g_alpha_neg);		/**< delta = g_alpha' + g_alpha_neg; here is P * P -> Add them !!! */
	
	NNModInv(beta_inv, msk->beta, param.m, NUMWORDS);	/**< beta_inv = 1/beta */
	ECC_assign(&tmp, delta);
	ECC_mul(delta, &tmp, beta_inv);					/**< d = d * beta_inverted */
	
	
	// replace old g_alpha in msk
	ECC_assign(&(msk->g_alpha), &(g_alpha_prime));
}

void cpabe_pub_update(NN2_NUMBER *g_hat_alpha_prime, cpabe_pub_t *pub) {
	NN2Assign(&(pub->g_hat_alpha), g_hat_alpha_prime, NUMWORDS);
}

void cpabe_prv_update(Point *delta, cpabe_prv_t *prv) {
	Point tmp;
	
	ECC_assign(&tmp, &(prv->d));
	ECC_add(&(prv->d), &tmp, delta);	/**< d' = d + delta; here is P * P -> Add them !!! */
	
}

/* *** Serializer ************************************************** */

/* byte */
static uint8_t* serialize_bytewise(uint8_t* b, uint8_t* data, uint16_t len) {
	uint16_t i;
	
	for( i = 0; i < len; i++ )
	{
		b[i] = data[i]; 
	}
	
	return b + len;
}

static uint8_t* unserialize_bytewise(uint8_t* b, uint8_t* data, uint16_t len) {
	uint16_t i;
	
	for( i = 0; i < len; i++ )
	{
		data[i] = b[i]; 
	}
	
	return b + len;
}

/* int */
static uint8_t* serialize_uint16(uint8_t* b, uint16_t data) {
	uint8_t len = sizeof(uint16_t);
	
	return serialize_bytewise(b, (uint8_t*) &data, len);
}

static uint8_t* unserialize_uint16(uint8_t* b, uint16_t* data) {
	uint8_t len = sizeof(uint16_t);
	
	return unserialize_bytewise(b, (uint8_t*) data, len);
}

/* NN_DIGIT */
static uint8_t* serialize_nndigit(uint8_t* b, NN_DIGIT* data) {
	uint16_t len = sizeof(NN_DIGIT) * NUMWORDS;
	
	return serialize_bytewise(b, (uint8_t*) data, len);
}

static uint8_t* unserialize_nndigit(uint8_t* b, NN_DIGIT* data) {
	uint16_t len = sizeof(NN_DIGIT) * NUMWORDS;
	
	return unserialize_bytewise(b, (uint8_t*) data, len);
}

/* Point */
static uint8_t* serialize_point(uint8_t* b, Point data) {
	uint16_t len = sizeof(NN_DIGIT) * NUMWORDS;
	uint8_t * pos;
	
	pos = serialize_bytewise(b, (uint8_t*) data.x, len);
	return serialize_bytewise(pos, (uint8_t*) data.y, len);
}

static uint8_t* unserialize_point(uint8_t* b, Point* data) {
	uint16_t len = sizeof(NN_DIGIT) * NUMWORDS;
	uint8_t * pos;
	
	pos = unserialize_bytewise(b, (uint8_t*) data->x, len);
	return unserialize_bytewise(pos, (uint8_t*) data->y, len);
}

/* NN2_NUMBER */
static uint8_t* serialize_nn2number(uint8_t* b, NN2_NUMBER data) {
	uint16_t len = sizeof(NN_DIGIT) * NUMWORDS;
	uint8_t * pos;
	
	pos = serialize_bytewise(b, (uint8_t*) data.r, len);
	return serialize_bytewise(pos, (uint8_t*) data.i, len);
}

static uint8_t* unserialize_nn2number(uint8_t* b, NN2_NUMBER* data) {
	uint16_t len = sizeof(NN_DIGIT) * NUMWORDS;
	uint8_t * pos;
	
	pos = unserialize_bytewise(b, (uint8_t*) data->r, len);
	return unserialize_bytewise(pos, (uint8_t*) data->i, len);
}

/* String */
static uint8_t* serialize_string(uint8_t* b, char* data) {
	uint8_t len = strlen(data)+1;
	
	b[0] = len;
	b++;
	return serialize_bytewise(b, (uint8_t*) data, len);
}

static uint8_t* unserialize_string(uint8_t* b, char* data) {
	uint8_t len = b[0];
	
	b++;
	return unserialize_bytewise(b, (uint8_t*) data, len);
}


/* pub key */
uint8_t* cpabe_pub_serialize( cpabe_pub_t* pub )
{
	uint8_t* b = (uint8_t *) malloc(8 * sizeof(NN_DIGIT) * NUMWORDS);
	uint8_t* pos;
	
	pos = serialize_point(b, pub->g);
	pos = serialize_point(pos, pub->h);
	pos = serialize_point(pos, pub->gp);
	serialize_nn2number(pos, pub->g_hat_alpha);
	
	return b;
}

void cpabe_pub_unserialize( cpabe_pub_t* pub, uint8_t* b, int f )
{
	uint8_t* pos;
	
	pos = unserialize_point(b, &(pub->g));
	pos = unserialize_point(pos, &(pub->h));
	pos = unserialize_point(pos, &(pub->gp));
	unserialize_nn2number(pos, &(pub->g_hat_alpha));
	
	if( f )
		free(b);
}

/* msk key */
uint8_t* cpabe_msk_serialize( cpabe_msk_t* msk )
{
	uint8_t* b = (uint8_t*) malloc(3 * sizeof(NN_DIGIT) * NUMWORDS);
	uint8_t* pos;
	
	pos = serialize_nndigit(b, msk->beta);
	serialize_point(pos, msk->g_alpha);
	
	return b;
}

void cpabe_msk_unserialize( cpabe_msk_t* msk, uint8_t* b, int f )
{
	uint8_t* pos;
	
	pos = unserialize_nndigit(b, msk->beta);
	unserialize_point(pos, &(msk->g_alpha));
	
	if( f )
		free(b);
}

/* prv key */
uint8_t
cpabe_prv_serialize( uint8_t* b, cpabe_prv_t* prv )
{
	uint8_t i;
	uint16_t list_len = list_length(prv->comps);
	//	uint8_t* b = (uint8_t*) malloc();
	uint8_t* pos;
	uint16_t size = 2 + 2 * (NUMWORDS * sizeof(NN_DIGIT));
	
	pos = serialize_point(b, prv->d);
	pos = serialize_uint16( pos, list_len);
	
	for( i = 0; i < list_len; i++ )
	{
		pos = serialize_string(pos, ((cpabe_prv_comp_t*) list_index(prv->comps, i))->attr);
		pos = serialize_point( pos, ((cpabe_prv_comp_t*) list_index(prv->comps, i))->d);
		pos = serialize_point( pos, ((cpabe_prv_comp_t*) list_index(prv->comps, i))->dp);
		size += 4 * (NUMWORDS * sizeof(NN_DIGIT)) + strlen(((cpabe_prv_comp_t*) list_index(prv->comps, i))->attr) + 2;
	}
	
	return size; // return size
}

void cpabe_prv_unserialize( cpabe_prv_t* prv, uint8_t* b, int f )
{
	int i;
	uint16_t len;
	cpabe_prv_comp_t* c;
	uint8_t* pos;
	
	pos = unserialize_point(b, &(prv->d));
	
	pos = unserialize_uint16(pos, &len);
	
	// init
	MYLIST(prv->comps);
	list_init(prv->comps);
	
	for( i = 0; i < len; i++ )
	{
		c = (cpabe_prv_comp_t *) memb_alloc(&prv_comps_m);
		c->attr = (char *) malloc(pos[0]);
		
		pos = unserialize_string(pos, c->attr);
		pos = unserialize_point( pos, &(c->d));
		pos = unserialize_point( pos, &(c->dp));
		
		list_add(prv->comps, c);
	}
	
	if( f )
		free(b);
}

/* policy */
uint8_t* serialize_policy( uint8_t* b, cpabe_policy_t *p, uint16_t * size )
{
	int i;
	uint8_t* pos;
	cpabe_policy_t *elem;
	
	*size += 4;
	
	pos = serialize_uint16(b, p->k);
	
	pos = serialize_uint16(pos, list_length(p->children));
	
	if( list_length(p->children) == 0 )
	{
		pos = serialize_string(pos, p->attr);
		pos = serialize_point( pos, p->c);
		pos = serialize_point( pos, p->cp);
		
		*size += 4 * (NUMWORDS * sizeof(NN_DIGIT)) + strlen(p->attr) + 2;
	}
	else {
		for( i = 0; i < list_length(p->children); i++ ) {
			if ((elem = list_index(p->children, i)) != NULL) {
				pos = serialize_policy(pos, elem, size);
			}
			
		}
	}
	
	return pos;
}

cpabe_policy_t*
unserialize_policy( uint8_t* b, cpabe_policy_t* p )
{
	int i;
	uint16_t n;
	cpabe_policy_t* p_next;
	uint8_t *pos;
	
	
	pos = unserialize_uint16(b, &(p->k));
	
	// init
	p->attr = 0;
	MYLIST(p->children);
	list_init(p->children);
	MYLIST(p->satl);
	list_init(p->satl);
	
	pos = unserialize_uint16(pos, &n);
	if( n == 0 )
	{
		p->attr = (char *) malloc(pos[0]);
		
		pos = unserialize_string(pos, p->attr);
		pos = unserialize_point(pos, &(p->c));
		pos = unserialize_point(pos, &(p->cp));
	}
	else
		for( i = 0; i < n; i++ ) {
			p_next = (cpabe_policy_t*) memb_alloc(&enc_policy_m);
			list_add(p->children, unserialize_policy(pos, p_next));
		}
	
	return p;
}

/* cph text */
uint8_t cpabe_cph_serialize( uint8_t* b, cpabe_cph_t* cph )
{
	//	uint8_t* b = (uint8_t*) malloc();
	//uint8_t* old_pos = b;
	uint8_t* pos;
	uint16_t size = 4 * (NUMWORDS * sizeof(NN_DIGIT));
	
	pos = serialize_nn2number(b, cph->cs);
	pos = serialize_point(pos, cph->c);
	
	pos = serialize_policy( pos, list_head(cph->p), &size);
	
	return size; // TODO: return size
}

void cpabe_cph_unserialize( cpabe_cph_t* cph, uint8_t* b, int f )
{
	uint8_t* pos;
	
	MYLIST(cph->p);
	list_init(cph->p);
	list_add(cph->p, (cpabe_policy_t*) memb_alloc(&enc_policy_m));
	
	pos = unserialize_nn2number(b, &(cph->cs));
	pos = unserialize_point(pos, &(cph->c));
	
	unserialize_policy(pos, list_head(cph->p));
	
	if( f )
		free(b);
}


/* *** free *******************************************************************/

/*
 void
 cpabe_pub_free( cpabe_pub_t* pub )
 {
 element_clear(pub->g);
 element_clear(pub->h);
 element_clear(pub->gp);
 element_clear(pub->g_hat_alpha);
 pairing_clear(pub->p);
 free(pub->pairing_desc);
 free(pub);
 }
 */
/*
	void
	cpabe_msk_free( cpabe_msk_t* msk )
	{
	element_clear(msk->beta);
	element_clear(msk->g_alpha);
	free(msk);
	}
*/
void cpabe_prv_free( cpabe_prv_t* prv )
{
	/*	int i;
	 
	 element_clear(prv->d);
	 
	 for( i = 0; i < prv->comps->len; i++ )
	 {
	 cpabe_prv_comp_t c;
	 
	 c = g_array_index(prv->comps, cpabe_prv_comp_t, i);
	 free(c.attr);
	 element_clear(c.d);
	 element_clear(c.dp);
	 }
	 
	 g_array_free(prv->comps, 1);
	 
	 free(prv);
	 */
	cpabe_prv_comp_t *c;
	
	while (list_length(prv->comps) > 0) {
		c = list_pop(prv->comps);
		
		if (c != NULL) {
			/**/ mem_free_count += sizeof(c->attr); /**/
			free(c->attr);
			memb_free(&prv_comps_m, c); /**/ memb_comp_free_count++; /**/
		}
	}
	
	
}

void cpabe_policy_free( cpabe_policy_t* p )
{
	if( p->attr )
	{
		/**/ mem_free_count += sizeof(p->attr); /**/
		free(p->attr);
		//		element_clear(p->c);
		//		element_clear(p->cp);
	
		while (list_length(p->satl) > 0) {
			satl_int_t* sat = list_pop(p->satl);
		
			if (sat != NULL) {
				/**/ mem_free_count += sizeof(satl_int_t); /**/
				free(sat);
			}
		}
	}
	
	while(list_length(p->children) > 0 ) {
		cpabe_policy_t* pol = list_pop(p->children);
		cpabe_policy_free(pol);
	}
	
	memb_free(&enc_policy_m, p); /**/ memb_policy_free_count++; /**/
}

void cpabe_cph_free( cpabe_cph_t* cph )
{
	//	element_clear(cph->cs);
	//	element_clear(cph->c);
	cpabe_policy_free(list_head(cph->p));
}



