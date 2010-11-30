/*
 *  cp-abe.h
 *  
 *
 *  Created by Daniel Jacobi on 19.05.10.
 *  Copyright 2010 TU Darmstadt. All rights reserved.
 *
 */

#ifndef _CP_ABE_H_
#define _CP_ABE_H_

#include "NN.h"
#include "ECC.h"
#include <list.h>

/*
 * A public key.
 */
typedef struct cpabe_pub_s {
//	char* pairing_desc;
//	pairing_t p;
	Point g;           /* G_1 */
	Point h;           /* G_1 */
	Point f;           /* G_1 */
//	Point gp;          /* G_2 */ // TODO: what is this for?!
	NN_DIGIT g_hat_alpha[NUMWORDS]; /* G_T */
} cpabe_pub_t;

/*
 * A master secret key.
 */
typedef struct cpabe_msk_s {
	NN_DIGIT beta[NUMWORDS];    /* Z_r */
	Point g_alpha; /* G_2 */
} cpabe_msk_t;


typedef struct cpabe_prv_comp_s {
	struct cpabe_prv_comp_s *next;
	/* these actually get serialized */
	char* attr;
	Point d;  /* G_2 */
	Point dp; /* G_2 */
	
	/* only used during dec (only by dec_merge) */
	//	int used;
	//	NN_DIGIT z;  /* G_1 */
	//	NN_DIGIT zp; /* G_1 */
} cpabe_prv_comp_t;

/*
 * A private key.
 */
typedef struct cpabe_prv_s {
	Point d;   /* G_2 */
	list_t comps; /* list of cpabe_prv_comp_t */
	void * comps_list; /* used by list lib */
} cpabe_prv_t;


typedef struct cpabe_polynomial_s {
	int deg;
	/* coefficients from [0] x^0 to [deg] x^deg */
	NN_DIGIT *coef; /* G_T (of length deg + 1) */
} cpabe_polynomial_t;

typedef struct satl_int_s {
	struct satl_int_s * next;
	int k;
} satl_int_t;

typedef struct cpabe_policy_s {
	struct cpabe_policy_s* next;
	/* serialized */
	int k;            /* one if leaf, otherwise threshold */
	char* attr;       /* attribute string if leaf, otherwise null */
	Point c;      /* G_1, only for leaves */
	Point cp;     /* G_1, only for leaves */
	list_t children; /* list of struct cpabe_policy_s, len == 0 for leaves */
	void * children_list; /* used by list lib */
	
	/* only used during encryption */
	cpabe_polynomial_t* q;
	
	/* only used during decryption */
	int satisfiable;
	int min_leaves;
	int attri;
	list_t satl; // TODO: Replace GArray, list, type?
	void * satl_list; /* used by list lib */
} cpabe_policy_t;


/*
 A ciphertext. Note that this library only handles encrypting a
 single group element, so if you want to encrypt something bigger,
 you will have to use that group element as a symmetric key for
 hybrid encryption (which you do yourself).
 */
typedef struct cpabe_cph_s {
	NN_DIGIT cs[NUMWORDS]; /* G_T */
	Point c;  /* G_1 */
	list_t p;	/* tree of cpabe_policy_t */
	void * p_list; /* used by list lib */
} cpabe_cph_t;

/*
 Generate a public key and corresponding master secret key, and
 assign the *pub and *msk pointers to them. The space used may be
 later freed by calling cpabe_pub_free(*pub) and
 cpabe_msk_free(*msk).
 */
extern void cpabe_setup(cpabe_pub_t *pub, cpabe_msk_t *msk);

/*
 Generate a private key with the given set of attributes. The final
 argument should be a null terminated array of pointers to strings,
 one for each attribute.
 */
extern void cpabe_keygen(cpabe_prv_t *prv, cpabe_pub_t pub, cpabe_msk_t msk, char** attributes);

/*
 Pick a random group element and encrypt it under the specified
 access policy. The resulting ciphertext is returned and the
 NN_DIGIT given as an argument (which need not be initialized) is
 set to the random group element.
 
 After using this function, it is normal to extract the random data
 in m using the pbc functions element_length_in_bytes and
 NN_DIGITo_bytes and use it as a key for hybrid encryption.
 
 The policy is specified as a simple string which encodes a postorder
 traversal of threshold tree defining the access policy. As an
 example,
 
 "foo bar fim 2of3 baf 1of2"
 
 specifies a policy with two threshold gates and four leaves. It is
 not possible to specify an attribute with whitespace in it (although
 "_" is allowed).
 
 Numerical attributes and any other fancy stuff are not supported.
 
 Returns null if an error occured, in which case a description can be
 retrieved by calling cpabe_error().
 */
extern void cpabe_enc(cpabe_cph_t *cph, cpabe_pub_t pub, NN_DIGIT m[NUMWORDS], char *policy);

/*
 Decrypt the specified ciphertext using the given private key,
 filling in the provided element m (which need not be initialized)
 with the result.
 
 Returns true if decryption succeeded, false if this key does not
 satisfy the policy of the ciphertext (in which case m is unaltered).
 */
extern int cpabe_dec(cpabe_pub_t pub, cpabe_prv_t prv, cpabe_cph_t cph, NN_DIGIT m[NUMWORDS]);

#endif
