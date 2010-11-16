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
#include "sha1.h"

static TPParams param;


MEMB(prv_comps_m, cpabe_prv_comp_t, 5);						/**< This limits the number of attributes in the private key */
MEMB(enc_policy_m, cpabe_policy_t, 5);						/**< This limits the number of attributes in encrypted data */
MEMB(enc_polynomial_m, cpabe_polynomial_t, 5);				/**< This limits the number of attributes in encrypted data */

/* -- Utility functions ----------------------------------------------------- */

/*
 * @brief Splits a String at " ".
 */
char **strsplit(char *s) {
	char ** tokens = NULL;
	uint8_t i, tok_count = 0;
	char * tmp;
	
	// count number of tokens
	for (i = 0; i < strlen(s); i++) {
		if (s[i] == ' ')
			tok_count++;
	}
	
	// alloc base array
	if (tok_count > 0) {
		tokens = (char **) malloc(tok_count * sizeof(char *));
	}
	
	// reuse string?
	for (i = 0; i < tok_count; i++) {
		tmp = strchr(s, ' ');								/**< get pos of next ' ' char */
		if (tmp != NULL) {
			tmp[0] = '\0';									/**< replace ' ' by '\0', tmp points to rest of string */
			tmp++;
		}
		
		tokens[i] = s;
		s = tmp;
	}

	return tokens;
}

/**
 * @brief frees all memory used in toks
 */
void freesplit(char ** toks) {
	int8_t i = sizeof(toks) / sizeof(char*);
	for (; i >= 0 ; i--) {
		free(toks[i]);
	}
	free(toks);
}

/**
 * Generate random, non-zero b, with b = b mod c.
 * Copied from ECC_gen_private_key()
 * TODO: Verify
 */
static void NNModRandom (NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits) {
	NN_UINT order_digit_len, order_bit_len;
    bool done = FALSE;
    uint8_t ri;
    NN_DIGIT digit_mask;
	
    order_bit_len = NNBits(param.m, NUMWORDS); // TODO: m replaced r correct?
    order_digit_len = NNDigits(param.m, NUMWORDS); // --^

    while(!done){
		watchdog_periodic();
		
		for (ri=0; ri<order_digit_len; ri++){
#ifdef THIRTYTWO_BIT_PROCESSOR
			b[ri] = ((uint32_t)rand() << 16)^((uint32_t)rand());
#else
			b[ri] = (NN_DIGIT)rand();
#endif
		}
		
		for (ri=order_digit_len; ri<NUMWORDS; ri++){
			b[ri] = 0;
		}
		
		if (order_bit_len % NN_DIGIT_BITS != 0){
			digit_mask = MAX_NN_DIGIT >> (NN_DIGIT_BITS - order_bit_len % NN_DIGIT_BITS);
			b[order_digit_len - 1] = b[order_digit_len - 1] & digit_mask;
		}
		NNModSmall(b, c, NUMWORDS);
		
		if (NNZero(b, NUMWORDS) != 1)
			done = TRUE;
	}
}

/*
 * Assign 1. a = 1.
 */
void NNAssignOne(NN_DIGIT * a, NN_UINT digits)
{
    uint8_t i;
    
    for (i = 1; i < digits; i++)
		a[i] = 0;
    
	a[0] = 1;
}

/*
 * Assign Points a = b.
 */
static void ECC_assign(Point *P0, Point *P1) {
	NNAssign(P0->x, P1->x, NUMWORDS);
	NNAssign(P0->y, P1->y, NUMWORDS);
}

/*
 * @brief Return item at position index.
 * /
void * list_index(list_t list, uint8_t index) {
	struct list *l;
	int n = 0;
	
	for(l = *list; l != NULL; l = l->next) {
		if (n != index) {
			++n;
		} else {
			break;
		}

	}
	
	return l;
} */// TODO: ^- There is a bug and i don't see it!

/**
 * This replaces an sscanf call and parses the format "%dto%d".
 */
uint8_t sscanf_repl(char* tok, /*char * format,*/ uint8_t* k, uint8_t* n) {
	uint16_t i, pos = 0;
	char* param1;
	char* param2;
	uint8_t ret = 0;

	// find 'to'
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
			if (param1 == NULL) { free(param2);
			} else if (param2 == NULL) { free(param1);
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

/**
 * @brief generate public and master key pair for CP-ABE.
 *
 * @param pub Public key
 * @param msk Master key
 */
void cpabe_setup(cpabe_pub_t *pub, cpabe_msk_t *msk) {
	// ERROR if NULL pointer 
	if (pub == NULL || msk == NULL) {
//			printf("cpabe_setup - ERROR: pub or msk NULL! pub: %p, msk: %p", pub, msk);
	}
	
	NN_DIGIT alpha[NUMWORDS];
	NN_DIGIT beta_inv[NUMWORDS];

	// init ECC with curve params
	Params *p = ECC_get_param();
	get_param(p);
    ECC_init();
	
	// init TP with curve params
	get_TP_param(&param);
	
	NNModRandom(alpha, param.p, NUMWORDS);			/**< Random alpha */
 	NNModRandom(msk->beta, param.p, NUMWORDS);	/**< Random beta */
	ECC_assign(&(pub->g), &param.P);				/**< g = param.P */
//	NNModRandom(&(pub->gp), param.p, NUMWORDS);		// FIXME: Is this needed?
	
	ECC_mul(&(msk->g_alpha), &(pub->g), alpha);		/**< g_alpha = g * alpha */
	ECC_mul(&(pub->h), &(pub->g), msk->beta);	/**< h = g * beta */

	/** f = g * (1/beta) */
	NNModInv(beta_inv, msk->beta, param.p, NUMWORDS);
	ECC_mul(&(pub->f), &(pub->g), beta_inv);	
	
	TP_TatePairing(pub->g_hat_alpha, pub->g, msk->g_alpha);	/**< g_hat_alpha = e(g, g * alpha) */
	
}

/* ** CP-ABE Keygen ********************* */


/**
 * @brief Compute a Point related to a string.
 * @param h Resulting point
 * @param s String for point computation
 */
void point_from_string( Point* h, char* s )
{
	NN_DIGIT r[NUMWORDS/*SHA1HashSize*/]; // FIXME: does not work for curves smaller 160bit!
	SHA1Context context;
	NN_DIGIT tmp[NUMWORDS];
	
	SHA1_Reset(&context);
	SHA1_Update(&context, s, strlen(s));
	SHA1_Digest(&context, (uint8_t *)r);
	
	// compute y for given (hash) x value  
	NNMod(h->x, r, NUMWORDS, param.p, NUMWORDS);	/**< x = r */
	NNAssign(h->y, param.E.b, NUMWORDS);			/**< (y^2) = b */
	if (!param.E.a_zero) {
		if (param.E.a_one) {						/**< (y^2) += a * x */
			NNModAdd(h->y, h->y, r, param.p, NUMWORDS);
		} else {
			NNModMult(tmp, param.E.a, r, param.p, NUMWORDS);
			NNModAdd(h->y, h->y, tmp, param.p, NUMWORDS);
		}
	}
	NNModMult(tmp, r, r, param.p, NUMWORDS);
	NNModMult(tmp, tmp, r, param.p, NUMWORDS);
	NNModAdd(h->y, h->y, tmp, param.p, NUMWORDS);	/**< (y^2) += x^3 */
	NNModSqrRootOpt(h->y, h->y, param.p, NUMWORDS, NULL);	/**< y = sqrt(y^2) */
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
	uint i = 0;
	
	if (prv == NULL || &pub == NULL || &msk == NULL) {
		//			printf("cpabe_setup - ERROR: pub or msk NULL! pub: %p, msk: %p", pub, msk);
	}
	
	// init
	list_init(prv->comps);
	
	// compute
	NNModRandom(r, param.p, NUMWORDS);
	ECC_mul(&g_r, &pub.g, r);						/**< g_r = g * r */
	
	ECC_add(&(prv->d), &msk.g_alpha, &g_r);			/**< d = g_alpha + g_r; here is P * P -> Add them !!! */
	NNModInv(beta_inv, msk.beta, param.p, NUMWORDS);	/**< beta_inv = 1/beta */
	ECC_assign(&tmp, &(prv->d));
	ECC_mul(&(prv->d), &tmp, beta_inv);				/**< d = d * beta_inverted */
	
	// for all attributes generate params
printf("%p: %s \n", attributes, attributes[i]);
	while( attributes[i] != NULL )					// TODO: Code is not called: problem with attributes reference!
	{
		c = (cpabe_prv_comp_t *) memb_alloc(&prv_comps_m);

printf("before: %s\n", attributes[i]);		
		c->attr = attributes[i++];
printf("after: %s\n", attributes[i]);		
		
 		point_from_string(&h_rp, c->attr);
 		NNModRandom(rp, param.p, NUMWORDS);
		
		ECC_assign(&tmp, &h_rp);
		ECC_mul(&h_rp, &tmp, rp);					/**< h_rp = h * rp */
			
		ECC_add(&(c->d), &g_r, &h_rp);				/**< d = g_r + h_rp */
		ECC_mul(&(c->dp), &pub.g, rp);				/**< dp = g * rp */
		
		// clear h_rb, rp (not needed)
		
		list_add(prv->comps, c);
	}
}


/* ** CP-ABE Encryption ********************* */


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
	list_init(p->children);
	p->q = 0;
	
	return p;
}

void
parse_policy_postfix( cpabe_cph_t *cph, char* s )
{
	char** toks;
	char** cur_toks;
	char*  tok;
	toks     = strsplit(s);
	cur_toks = toks;
	cpabe_policy_t* node;
	
	list_init(cph->p);
	
	while( *cur_toks )												// FIXME: same as with keygen, but should work
	{
		uint8_t i, k, n;
		
		tok = *(cur_toks++);
		
		if( !*tok )
			continue;
		
		if( sscanf_repl(tok, /*"%dof%d",*/ &k, &n) != 2 )					// sscanf not in library, replaced
		// push leaf token 
			list_push(cph->p, base_node(1, tok));
		else
		{
			// parse "kofn" operator 
			
			if( k < 1 )
			{
//				raise_error("error parsing \"%s\": trivially satisfied operator \"%s\"\n", s, tok);
				return;
			}
			else if( k > n )
			{
//				raise_error("error parsing \"%s\": unsatisfiable operator \"%s\"\n", s, tok);
				return;
			}
			else if( n == 1 )
			{
//				raise_error("error parsing \"%s\": identity operator \"%s\"\n", s, tok);
				return;
			}
			else if( n > list_length(cph->p) )
			{
//				raise_error("error parsing \"%s\": stack underflow at \"%s\"\n", s, tok);
				return;
			}
			
			// pop n things and fill in children 
			node = base_node(k, 0);
			list_init(node->children);
			for( i = 0; i <= n - 1; i++ )
				list_add(node->children, list_pop(cph->p));
			
			// push result 
			list_push(cph->p, node);
		}
	}
	
	if( list_length(cph->p) > 1 )
	{
		//raise_error("error parsing \"%s\": extra tokens left on stack\n", s);
		return;
	}
	else if( list_length(cph->p) < 1 )
	{
		//raise_error("error parsing \"%s\": empty policy\n", s);
		return;
	}
		
 	freesplit(toks);
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
 		NNModRandom(&(q->coef[i * NUMWORDS]), param.p, NUMWORDS);			// FIXME: Array addressing correct?
	
	return q;
}

void
eval_poly( NN_DIGIT r[NUMWORDS], cpabe_polynomial_t* q, NN_DIGIT x[NUMWORDS] )
{
	int i;
	NN_DIGIT s[NUMWORDS], t[NUMWORDS];
		
	NNAssignZero(r, NUMWORDS);
	NNAssignOne(t, NUMWORDS);
	
	for( i = 0; i < q->deg + 1; i++ )
	{
		// r += q->coef[i] * t 
		NNModMult(s, &(q->coef[i * NUMWORDS]), t, param.p, NUMWORDS);
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
printf("41\n");	
	
	if( list_length(p->children) == 0 )
	{		
		point_from_string(&h, p->attr);
printf("411\n");	
		ECC_mul(&(p->c),  &pub.g, &(p->q->coef[0]));
printf("412\n");	
		ECC_mul(&(p->cp), &h,     &(p->q->coef[0]));
printf("42\n");	
	}
	else {
		i = 1;
		for( cp = list_head(p->children); cp != NULL; cp = cp->next )
			{
				NNAssign(r, &i, NUMWORDS);				/**< Assign i+1 to r */
				eval_poly(t, p->q, r);
				fill_policy(cp, pub, t);
				
				i++;
			}
printf("43\n");	
	}
}

/**
 * @brief generate symetric encryption key m with public key and a specified policy.
 */
void cpabe_enc(cpabe_cph_t *cph, cpabe_pub_t pub, NN_DIGIT m[NUMWORDS], char *policy) {
 	NN_DIGIT s[NUMWORDS];
	
	// initialize 
	
	parse_policy_postfix(cph, policy);
printf("1\n");	
	// compute 
	
 	NNModRandom(m, param.p, NUMWORDS);
 	NNModRandom(s, param.p, NUMWORDS);
printf("2\n");	
	NNModExp(cph->cs,  pub.g_hat_alpha, s, NUMWORDS, param.p, NUMWORDS);	// FIXME: g_hat_alpha pow s ???
	NNModMult(cph->cs, cph->cs, m, param.p, NUMWORDS);		/**< cs = cs * m */
printf("3\n");	
	
	ECC_mul(&(cph->c), &(pub.h), s);						/**< c = h * s */
printf("4\n");	
	
	fill_policy((cpabe_policy_t *) &(cph->p), pub, s);
printf("5\n");	
}


/* ** CP-ABE Decryption ********************* */

/*
//#error Decryption not ready. You have to finish the porting!
void
check_sat( cpabe_policy_t* p, cpabe_prv_t* prv )
{
	int i, l;
	
	p->satisfiable = 0;
	if( list_length(p->children) == 0 )
	{
		for( i = 0; i < list_length(prv->comps); i++ )
			if( !strcmp(((cpabe_prv_comp_t *) list_index(prv->comps, i))->attr,	
						p->attr) )
			{
				p->satisfiable = 1;
				p->attri = i;
				break;
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

void
pick_sat_min_leaves( cpabe_policy_t* p, cpabe_prv_t* prv )
{
	int i, k, l;
	int* c;
	
	assert(p->satisfiable == 1);
	
	if( list_length(p->children) == 0 )
		p->min_leaves = 1;
	else
	{
		for( i = 0; i < list_length(p->children); i++ )
			if( ((cpabe_policy_t*) list_index(p->children, i))->satisfiable )
				pick_sat_min_leaves(list_index(p->children, i), prv);
		
		c = malloc(sizeof(int) * list_length(p->children));
		for( i = 0; i < list_length(p->children); i++ )
			c[i] = i;
		
//		cur_comp_pol = p;																// ???
		qsort(c, list_length(p->children), sizeof(int), cmp_int);	
		
		p->satl = g_array_new(0, 0, sizeof(int));										// TODO: Reimplement Array stuff
		p->min_leaves = 0;
		l = 0;
		
		for( i = 0; i < list_length(p->children) && l < p->k; i++ )
			if( ((cpabe_policy_t*) list_index(p->children, c[i]))->satisfiable )
			{
				l++;
				p->min_leaves += ((cpabe_policy_t*) list_index(p->children, c[i]))->min_leaves;
				k = c[i] + 1;
				g_array_append_val(p->satl, k);											// TODO: What does this do?
			}
		assert(l == p->k);
		
		free(c);
	}
}

void
lagrange_coef( NN_DIGIT * r, GArray* s, int i )		// TODO: Array stuff
{
	int j, k;
	NN_DIGIT t[NUMWORDS];
		
	NNAssignOne(r);
	for( k = 0; k < s->len; k++ )
	{
		j = g_array_index(s, int, k);
		if( j == i )
			continue;
		NNAssignMod(t, - j, param.p, NUMWORDS);									// TODO: How to do?! -> negative number!
		NNModMul(r, r, t, param.p, NUMWORDS); / * num_muls++; * /
		NNAssignMod(t, i - j, param.p, NUMWORDS);		
		NNModInv(t, t, param.p, NUMWORDS);
		NNModMul(r, r, t, param.p, NUMWORDS); / * num_muls++; * /
	}
	
	//element_clear(t);
}

void
dec_leaf_flatten( NN_DIGIT * r, NN_DIGIT * exp,
				 cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub )
{
	cpabe_prv_comp_t* c;
	NN_DIGIT s[NUMWORDS];
	NN_DIGIT t[NUMWORDS];
	
	c = (cpabe_prv_comp_t *) list_index(prv->comps, p->attri);
		
	// TODO: pairing_apply(s, p->c,  c->d,  pub->p); / * num_pairings++; * /
	// TODO: pairing_apply(t, p->cp, c->dp, pub->p); / * num_pairings++; * /
	NNModInv(t, t, param.p, NUMWORDS);
	NNModMul(s, s, t, param.p, NUMWORDS); / * num_muls++; * /
	NNModExp(s, s, exp, param.p, NUMWORDS); / * num_exps++; * /
	
	NNModMul(r, r, s, param.p, NUMWORDS); / * num_muls++; * /
	
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
	
	for( i = 0; i < p->satl->len; i++ )
	{
 		lagrange_coef(t, p->satl, g_array_index(p->satl, int, i));				// TODO: GArray stuff
		NNModMul(expnew, exp, t, param.p, NUMWORDS); / * num_muls++; * /
		dec_node_flatten(r, expnew, list_index
						 (p->children, g_array_index(p->satl, int, i) - 1), prv, pub);	// TODO: GArray stuff
	}
	
	//element_clear(t);
	//element_clear(expnew);
}

void
dec_node_flatten( NN_DIGIT * r, NN_DIGIT * exp,
				 cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub )
{
	assert(p->satisfiable);
	if( list_length(p->children) == 0 )
		dec_leaf_flatten(r, exp, p, prv, pub);
	else
		dec_internal_flatten(r, exp, p, prv, pub);
}

void
dec_flatten( NN_DIGIT * r, cpabe_policy_t* p, cpabe_prv_t* prv, cpabe_pub_t* pub )
{
	NN_DIGIT one[NUMWORDS];
		
	NNAssignOne(one);
	NNAssignOne(r);
	
	dec_node_flatten(r, one, p, prv, pub);
	
	//element_clear(one);
}



//int cpabe_dec(cpabe_pub_t pub, cpabe_prv_t prv, cpabe_cph_t cph, uint8_t *m) {
//	NN_DIGIT t[NUMWORDS];
//	
//	check_sat(cph->p, prv);							// check properties saturation
//	if( !cph->p->satisfiable )
//	{
//		//raise_error("cannot decrypt, attributes in key do not satisfy policy\n");
//		return 0;
//	}
//	
//	/ * 	if( no_opt_sat ) * /
//	/ * 		pick_sat_naive(cph->p, prv); * /
//	/ * 	else * /
//	pick_sat_min_leaves(cph->p, prv);				
//	
//	/ * 	if( dec_strategy == DEC_NAIVE ) * /
//	/ * 		dec_naive(t, cph->p, prv, pub); * /
//	/ * 	else if( dec_strategy == DEC_FLATTEN ) * /
//	dec_flatten(t, cph->p, prv, pub);
//	/ * 	else * /
//	/ * 		dec_merge(t, cph->p, prv, pub); * /
//	
//	NN_ModMult(m, cph->cs, t, param.p, NUMWORDS); / * num_muls++; * /
//	
//	// TODO: pairing_apply(t, cph->c, prv->d, pub->p); / * num_pairings++; * /
//	NNModInv(t, t, param.p, NUMWORDS);
//	NN_ModMult(m, m, t, param.p, NUMWORDS); / * num_muls++; * / 
//	
//	return 1;
//}
*/

