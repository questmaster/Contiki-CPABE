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
#include "cp-abe.h"
#include "TP.h"
#include "qsort.h"
#include "sha1.h"

static TPParams param;
static cpabe_policy_t* cur_comp_pol;


MEMB(prv_comps_m, cpabe_prv_comp_t, 5);						/**< This limits the number of attributes in the private key */
MEMB(enc_policy_m, cpabe_policy_t, 5);						/**< This limits the number of attributes in encrypted data */
MEMB(enc_polynomial_m, cpabe_polynomial_t, 10);				/**< This limits the number of attributes in encrypted data */

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
	tmp = (char *) malloc((tok_count+1) * sizeof(char *) + strlen(s) + 1);		// FIXME: memb?
	tokens = (char **) tmp;
	tmp = tmp + ((tok_count+1) * sizeof(char *));
	memcpy(tmp, s, strlen(s) + 1);
							  
	// reuse string?
	tokens[0] = tmp;
	for (i = 1; i < tok_count; i++) {
		tmp1 = strchr(tmp, ' ');						/**< get pos of next ' ' char */
		if (tmp1 != NULL) {
			*tmp1 = 0;									/**< replace ' ' by '\0', tmp points to rest of string */
			tmp = ++tmp1;
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
	int8_t i = sizeof(toks) / sizeof(char*);
	for (; i >= 0 ; i--) {
		free(toks[i]);
	}
	free(toks);
}

/*
 * @brief Return item at position index.
 */
static void * list_index(list_t list, uint8_t index) {
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
		param1 = (char *) malloc(pos+1);						// FIXME: memb?
		param2 = (char *) malloc(strlen(tok) - pos - 2);		// FIXME: memb?
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


/* -- CP-ABE functions ------------------------------------------------------ */

/**
 * @brief init memory structures needed for CP-ABE.
 */
void cpabe_init() {
	memb_init(&prv_comps_m);
	memb_init(&enc_policy_m);
	memb_init(&enc_polynomial_m);
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
#not computed# pub->g_hat_alpha.i 11 e a6 b7 75 fb a8 92 f1 19 4f b1 3 14 d6 8 46 cb 68 b8 59 e8 3c 73 
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
#else
	NNModRandom(alpha, param.m, NUMWORDS);			/**< Random alpha */
 	NNModRandom(msk->beta, param.m, NUMWORDS);		/**< Random beta */
	ECC_Random(&(pub->g));		
	ECC_Random(&(pub->gp));	
#endif
	
	ECC_mul(&(msk->g_alpha), &(pub->gp), alpha);	/**< g_alpha = gp * alpha */
	ECC_mul(&(pub->h), &(pub->g), msk->beta);		/**< h = g * beta */

	
	TP_TatePairing(&(pub->g_hat_alpha), pub->g, msk->g_alpha);	/**< g_hat_alpha = e(g, gp * alpha) */
	
}
#endif

/* ** CP-ABE Keygen ********************* */

#ifdef CPABE_KEYGEN
/**
 * @brief Compute a Point related to a string.
 * @param h Resulting point
 * @param s String for point computation
 */
void point_from_string( Point* h, char* s )
{
	NN_DIGIT r[NUMWORDS/*SHA1HashSize*/]; // FIXME: not working with curves smaller 160bit!
	SHA1Context context;
	NN_DIGIT tmp[NUMWORDS];
	
	SHA1_Reset(&context);
	SHA1_Update(&context, s, strlen(s));
	SHA1_Digest(&context, (uint8_t *)r);
	
	// compute y for given (hash) x value  
	NNMod(tmp, r, NUMWORDS, param.p, NUMWORDS);	/**< x = r mod p */
	ECC_compY(h, tmp);
	
//	printf("DEBUG: point_from_string: %s\n", ECC_check_point(h) ? "ok" : "WRONG!");
}

/**
 * @brief Generate a secret key related to some attributes.
 *
 * @param prv Secret key related to attributes specified
 * @param pub Public key
 * @param msk Master key
 * @param attributes Attributes to include in secret key (should be element wise parsed attributes?)
 */
void cpabe_keygen(cpabe_prv_t *prv, cpabe_pub_t pub, cpabe_msk_t msk, char** attributes) {
	Point g_r;
	NN_DIGIT r[NUMWORDS];
	NN_DIGIT beta_inv[NUMWORDS];
	Point tmp;
	cpabe_prv_comp_t *c;
	Point h_rp;
	NN_DIGIT rp[NUMWORDS];
	
	if (prv == NULL || &pub == NULL || &msk == NULL) {
//			printf("cpabe_setup - ERROR: pub or msk NULL! pub: %p, msk: %p", pub, msk);
	}
	
	// init
	MYLIST(prv->comps);
	list_init(prv->comps);
	
	// compute
	NNModRandom(r, param.p, NUMWORDS);
	ECC_mul(&g_r, &pub.gp, r);						/**< g_r = gp * r */
	
	ECC_add(&(prv->d), &msk.g_alpha, &g_r);			/**< d = g_alpha + g_r; here is P * P -> Add them !!! */
	NNModInv(beta_inv, msk.beta, param.p, NUMWORDS);	/**< beta_inv = 1/beta */
	ECC_assign(&tmp, &(prv->d));
	ECC_mul(&(prv->d), &tmp, beta_inv);				/**< d = d * beta_inverted */
	
	// for all attributes generate params
	while( *attributes )
	{
		c = (cpabe_prv_comp_t *) memb_alloc(&prv_comps_m);

		c->attr = *(attributes++);
		
 		point_from_string(&h_rp, c->attr);
 		NNModRandom(rp, param.p, NUMWORDS);
		
		ECC_assign(&tmp, &h_rp);
		ECC_mul(&h_rp, &tmp, rp);					/**< h_rp = h * rp */
			
		ECC_add(&(c->d), &g_r, &h_rp);				/**< d = g_r + h_rp */
		ECC_mul(&(c->dp), &pub.g, rp);				/**< dp = g * rp */
		
		// TODO: clear h_rb, rp needed?
		
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
	
	p = (cpabe_policy_t*) memb_alloc(&enc_policy_m);
	p->k = k;
	if (s != NULL) {
		p->attr = (char *) malloc (strlen(s)+1);					// FIXME: memb?
		strncpy(p->attr, s, strlen(s));								// replaced strdup()
	} else {
		p->attr = 0;
	}
	MYLIST(p->children);
	list_init(p->children);
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
//				printf("error parsing \"%s\": trivially satisfied operator \"%s\"\n", s, tok);
				return 1;
			}
			else if( k > n )
			{
//				printf("error parsing \"%s\": unsatisfiable operator \"%s\"\n", s, tok);
				return 1;
			}
			else if( n == 1 )
			{
//				printf("error parsing \"%s\": identity operator \"%s\"\n", s, tok);
				return 1;
			}
			else if( n > list_length(cph->p) )
			{
//				printf("error parsing \"%s\": stack underflow at \"%s\" (n=%d > list_length=%d)\n", s, tok, n, list_length(cph->p));
				return 1;
			}
			
			// pop n things and fill in children 
			node = base_node(k, 0);
			for( i = 0; i < n; i++ )
				list_add(node->children, list_pop(cph->p));					// TODO: is order correct?! should be
			
			// push result 
			list_push(cph->p, node);
		}
	}
	
	if( list_length(cph->p) > 1 )
	{
//		printf("error parsing \"%s\": extra tokens left on stack\n", s);
		return 1;
	}
	else if( list_length(cph->p) < 1 )
	{
//		printf("error parsing \"%s\": empty policy\n", s);
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
	
	q = (cpabe_polynomial_t*) memb_alloc(&enc_polynomial_m);
	q->deg = deg;
	q->coef = (NN_DIGIT *) malloc(sizeof(NN_DIGIT) * NUMWORDS * (deg + 1));	// FIXME: memb?
	
	NNAssign(q->coef, zero_val, NUMWORDS);
	
	for( i = 1; i < q->deg + 1; i++ )
 		NNModRandom(q->coef + (i * NUMWORDS), param.p, NUMWORDS);			// FIXME: Array addressing correct?
	
	return q;
}

void
eval_poly( NN_DIGIT r[NUMWORDS], cpabe_polynomial_t* q, NN_DIGIT x[NUMWORDS] )
{
	int j;
	NN_DIGIT s[NUMWORDS];
	NN_DIGIT t[NUMWORDS];
		
	NNAssignZero(r, NUMWORDS);
	NNAssignOne(t, NUMWORDS);
	
	for( j = 0; j < q->deg + 1; j++ )
	{
		// r += q->coef[i] * t 
		NNModMult(s, q->coef + (j * NUMWORDS), t, param.p, NUMWORDS);
		NNModAdd(r, r, s, param.p, NUMWORDS);
		
		// t *= x 
		NNModMult(t, t, x, param.p, NUMWORDS);
	}
}

void
fill_policy(cpabe_policy_t *p, cpabe_pub_t pub, NN_DIGIT e[NUMWORDS]) {
	cpabe_policy_t* cp;
	int i;
	NN_DIGIT r[NUMWORDS];
	NN_DIGIT t[NUMWORDS];
	Point h;
		
	p->q = rand_poly(p->k - 1, e);
	
	if( list_length(p->children) == 0 )
	{		
		point_from_string(&h, p->attr);
		ECC_mul(&(p->c),  &pub.g, &(p->q->coef[0]));
		ECC_mul(&(p->cp), &h,     &(p->q->coef[0]));
	} else {
		i = 1;
		for(cp = list_head(p->children); cp != NULL; cp = cp->next )
			{
				NNAssignDigit(r, i, NUMWORDS);				/**< Assign i+1 to r */
				eval_poly(t, p->q, r);
				fill_policy(cp, pub, t);				/**< ...but supply element i */
				
				i++;
			}
	}
}

/**
 * @brief generate symetric encryption key m with public key and a specified policy.
 */
void cpabe_enc(cpabe_cph_t *cph, cpabe_pub_t pub, NN_DIGIT m[NUMWORDS], char *policy) {
 	NN_DIGIT s[NUMWORDS];
	
	// initialize 
	parse_policy_postfix(cph, policy);
	
	// compute 
 	NNModRandom(m, param.p, NUMWORDS);
 	NNModRandom(s, param.p, NUMWORDS);
	NNModExp(cph->cs,  pub.g_hat_alpha, s, NUMWORDS, param.p, NUMWORDS);	// TODO: g_hat_alpha pow s ???
	NNModMult(cph->cs, cph->cs, m, param.p, NUMWORDS);		/**< cs = cs * m */
	
	ECC_mul(&(cph->c), &(pub.h), s);						/**< c = h * s */
	
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

//#error ****** Still missing... ********* 

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
	
	if( list_length(p->children) == 0 )
		p->min_leaves = 1;
	else
	{
		for( i = 0; i < list_length(p->children); i++ )
			if( ((cpabe_policy_t*) list_index(p->children, i))->satisfiable )
				pick_sat_min_leaves(list_index(p->children, i), prv);
		
		c = malloc(sizeof(int) * list_length(p->children));						// FIXME: memb?
		for( i = 0; i < list_length(p->children); i++ )
			c[i] = i;
		
		cur_comp_pol = p;
		qsort(c, list_length(p->children), sizeof(int), cmp_int);
		
		MYLIST(p->satl);
		list_init(p->satl);
		p->min_leaves = 0;
		l = 0;
		
		for( i = 0; i < list_length(p->children) && l < p->k; i++ )
			if( ((cpabe_policy_t*) list_index(p->children, c[i]))->satisfiable )
			{
				l++;
				p->min_leaves += ((cpabe_policy_t*) list_index(p->children, c[i]))->min_leaves;
				k = c[i] + 1;
//				g_array_append_val(p->satl, k);
				alloc_k = (satl_int_t *) malloc(sizeof(satl_int_t));
				alloc_k->k = k;
				list_add(p->satl, alloc_k);
			}
//		assert(l == p->k);
		
		free(c);
	}
}



void
lagrange_coef( NN_DIGIT r[NUMWORDS], list_t s, int i )
{
	int j, k;
	NN_DIGIT zero[NUMWORDS];
	NN_DIGIT tmp1[NUMWORDS];
	NN_DIGIT tmp2[NUMWORDS];
	NN_DIGIT t[NUMWORDS];
		
	NNAssignZero(zero, NUMWORDS);
	
	NNAssignOne(r, NUMWORDS);
	for( k = 0; k < list_length(s); k++ )
	{
		j = ((satl_int_t *) list_index(s, k))->k;
		if( j == i )
			continue;

		NNAssignDigit(tmp1, j, NUMWORDS);									// FIXME: How to do?! -> negative number!
		NNModSub(t, zero, tmp1, param.p, NUMWORDS);									// t = -j

		NNModMult(r, r, t, param.p, NUMWORDS); /* num_muls++; */

		NNAssignDigit(tmp2, i, NUMWORDS);		
		NNModSub(t, tmp2, tmp1, param.p, NUMWORDS);									// t = i - j
		
		NNModInv(t, t, param.p, NUMWORDS);
		NNModMult(r, r, t, param.p, NUMWORDS); /* num_muls++; */
	}
	
	//element_clear(t);
}


/*
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


void
dec_leaf_flatten( NN_DIGIT * r, NN_DIGIT * exp,
				 cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub )
{
	cpabe_prv_comp_t* c;
	NN_DIGIT s[NUMWORDS];
	NN_DIGIT t[NUMWORDS];
	
	c = (cpabe_prv_comp_t *) list_index(prv->comps, p->attri);
		
	TP_TatePairing(s, p->c,  c->d); /* num_pairings++; */
	TP_TatePairing(t, p->cp, c->dp); /* num_pairings++; */
	NNModInv(t, t, param.p, NUMWORDS);
	NNModMult(s, s, t, param.p, NUMWORDS); /* num_muls++; */
	NNModExp(s, s, exp, NUMWORDS, param.p, NUMWORDS); /* num_exps++; */
	
	NNModMult(r, r, s, param.p, NUMWORDS); /* num_muls++; */
	
	//element_clear(s);		
	//element_clear(t);
}

void dec_node_flatten( NN_DIGIT * r, NN_DIGIT * exp,
					  cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub );

void
dec_internal_flatten( NN_DIGIT * r, NN_DIGIT * exp,
					 cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub ) 
{
	int i;
	NN_DIGIT t[NUMWORDS];
	NN_DIGIT expnew[NUMWORDS];
	
	for( i = 0; i < list_length(p->satl); i++ )
	{
 		lagrange_coef(t, p->satl, ((satl_int_t *) list_index(p->satl, i))->k);
		NNModMult(expnew, exp, t, param.p, NUMWORDS); /* num_muls++; */
		dec_node_flatten(r, expnew, list_index
						 (p->children, ((satl_int_t *) list_index(p->satl, i))->k - 1), prv, pub);
	}
	
	//element_clear(t);
	//element_clear(expnew);
}

void
dec_node_flatten( NN_DIGIT * r, NN_DIGIT * exp,
				 cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub )
{
//	assert(p->satisfiable);
	if( list_length(p->children) == 0 )
		dec_leaf_flatten(r, exp, p, prv, pub);
	else
		dec_internal_flatten(r, exp, p, prv, pub);
}

void
dec_flatten( NN_DIGIT * r, cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub )
{
	NN_DIGIT one[NUMWORDS];
		
	NNAssignOne(one, NUMWORDS);
	NNAssignOne(r, NUMWORDS);
	
	dec_node_flatten(r, one, p, prv, pub);
	
	//element_clear(one);
}



int cpabe_dec(cpabe_pub_t pub, cpabe_prv_t prv, cpabe_cph_t cph, NN_DIGIT m[NUMWORDS]) {
	NN_DIGIT t[NUMWORDS];
	
	check_sat(list_head(cph.p), &prv);							// check properties saturation
	if( !((struct cpabe_policy_s *) list_head(cph.p))->satisfiable )
	{
		printf("cannot decrypt, attributes in key do not satisfy policy\n");
		return 0;
	}
	
//	if( no_opt_sat ) 
//		pick_sat_naive(list_head(cph.p), &prv); 
//	else 
		pick_sat_min_leaves(list_head(cph.p), &prv);				
	
//	if( dec_strategy == DEC_NAIVE ) 
//		dec_naive(t, cph->p, prv, pub); // unported!
//	else if( dec_strategy == DEC_FLATTEN ) 
		dec_flatten(t, list_head(cph.p), &prv, &pub);
//	else 
//		dec_merge(t, cph->p, prv, pub); // unported!
	
	NNModMult(m, cph.cs, t, param.p, NUMWORDS); /* num_muls++; */
	
	TP_TatePairing(t, cph.c, prv.d); /* num_pairings++; */
	NNModInv(t, t, param.p, NUMWORDS);
	NNModMult(m, m, t, param.p, NUMWORDS); /* num_muls++; */ 
	
	return 1;
}
#endif

