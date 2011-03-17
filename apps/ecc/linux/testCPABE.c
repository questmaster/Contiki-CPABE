/*
 *  testCPABE.c
 *  
 *
 *  Created by Daniel Jacobi on 03.08.10.
 *  Copyright 2010 TU Darmstadt. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "NN.h"
#include "NN2.h"
#include "TP.h"
#include "ECC.h"
#include "cp-abe.h"

#define MAX_ROUNDS 10
#define CLOCK_SECOND CLOCKS_PER_SEC 

static cpabe_pub_t pub;
static cpabe_msk_t msk;
static cpabe_prv_t prv;
static cpabe_cph_t cph;
static uint8_t round_index = 0;
static char * attributes[8];
static char * policy;
static NN2_NUMBER m;
static NN2_NUMBER m2;

void val_print(char* name, NN_DIGIT *val) {
	int i;

	printf("%s", name);
	for (i = NUMWORDS-1; i >= 0; i--) {
#ifndef THIRTYTWO_BIT_PROCESSOR
		printf("%x ",val[i]);
#else
		printf("%x %x ", (uint16_t)(val[i] >> 16), (uint16_t) val[i]);
#endif
	}
	printf("\n");

}


/* scopes process */
int main(void)
{
    uint32_t time_s, time_f, dt0;
	
#ifdef CPABE_SMALL_SET
	/* small */
	policy = "attr1 1of1"; 
	attributes[0] = "attr1";
	attributes[1] = "attr2";
	attributes[2] = 0;
#elif defined (CPABE_MEDIUM_SET)
	/* medium */
	policy = "attr1 attr3 attr9 attr4 2of4"; // For MSP430 only one sub level possible! 
											 // -> most probably the stack is full and the node resets...
	attributes[0] = "attr1";
	attributes[1] = "attr2";
	attributes[2] = "attr3";
	attributes[3] = "attr4";
	attributes[4] = "attr5";
	attributes[5] = 0;
#elif defined (CPABE_LARGE_SET)
	/* large */
	policy = "attr4_expint08_xxxxx1xx attr4_expint08_xxxxxx1x attr4_expint08_1xxxxxxx attr4_expint08_x1xxxxxx attr4_expint08_xx1xxxxx attr4_expint08_xxx1xxxx attr4_expint08_xxxx1xxx 2of7"; 
	attributes[0] = "attr1";
	attributes[1] = "attr2";
	attributes[2] = "attr3";
	attributes[3] = "attr4_expint08_xxxxxx1x";
	attributes[4] = "attr4_expint08_xxxxx1xx";
	attributes[5] = "attr6";
	attributes[6] = "attr7";
	attributes[7] = 0;
#endif	
	
	
	printf("CP-ABE tester process started\n");
	printf("entering loop. CLOCK_SECOND=%d\n", CLOCK_SECOND);
	
	do {
		
#ifdef CPABE_SETUP
	printf("CPABE_setup(%d)\n", round_index);
	time_s = clock();
	
	cpabe_setup(&pub, &msk);

	time_f = clock();
	dt0 = time_f - time_s;
	printf("CPABE_setup(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));

	/* CP-ABE Keys */
#ifdef CPABE_DEBUG
	val_print("CPABE_msk_g-alpha_x: ", msk.g_alpha.x);
	val_print("CPABE_msk_g-alpha_y: ", msk.g_alpha.y);
	val_print("CPABE_pub_h_x: ", pub.h.x);
	val_print("CPABE_pub_h_y: ", pub.h.y);
	val_print("CPABE_pub_g-hat-alpha_r: ", pub.g_hat_alpha.r);
	val_print("CPABE_pub_g-hat-alpha_i: ", pub.g_hat_alpha.i);
#endif
#endif
	
#ifdef CPABE_KEYGEN
		printf("CPABE_keygen(%d)\n", round_index);
		time_s = clock();
		cpabe_keygen(&prv, pub, msk, attributes);

		time_f = clock();
		dt0 = time_f - time_s;
		printf("CPABE_keygen(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));

#ifdef CPABE_DEBUG
		val_print("CPABE_prv_d_x: ", prv.d.x);
		val_print("CPABE_prv_d_y: ", prv.d.y);
#endif
#endif
#ifdef CPABE_ENCRYPTION		
		printf("CPABE_enc(%d) \n", round_index);
		printf("enc policy: %s\n", policy);
		time_s = clock();
		
		cpabe_enc(&cph, pub, &m, policy); 
		
		time_f = clock();
		dt0 = time_f - time_s;
		printf("CPABE_enc(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));
		
		val_print("m.r  (plain): ", m.r);
		val_print("m.i  (plain): ", m.i);
#ifdef CPABE_DEBUG
		val_print("CPABE_cph_cs_r: ", cph.cs.r);
		val_print("CPABE_cph_cs_i: ", cph.cs.i);
		val_print("CPABE_cph_c_x: ", cph.c.x);
		val_print("CPABE_cph_c_y: ", cph.c.y);
#endif
#endif
#ifdef CPABE_DECRYPTION
		printf("CPABE_dec(%d) \n", round_index);
		time_s = clock();
		
		cpabe_dec(pub, prv, cph, &m2); 
		
		time_f = clock();
		dt0 = time_f - time_s;
		printf("CPABE_dec(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));

		val_print("m2.r (plain): ", m2.r);
		val_print("m2.i (plain): ", m2.i);
#endif
						
		// free dynamic memory
		cpabe_prv_free(&prv);
		cpabe_cph_free(&cph);
		
		round_index++;
	} while(round_index < MAX_ROUNDS);

	printf("Done.\n");

	return 0;
}


