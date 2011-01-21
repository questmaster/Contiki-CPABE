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

#include "testCPABE.h"
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
static char * attributes[4];
static char * policy;
static NN2_NUMBER m;
static NN2_NUMBER m2;

/* scopes process */
int main(void)
{
	int8_t i = 0;
    uint32_t time_s, time_f, dt0;
	
	policy = "attr1 attr3 2of2"; 
	attributes[0] = "attr1";
	attributes[1] = "attr2";
	attributes[2] = "attr3";
	attributes[3] = 0;
		
	printf("CP-ABE tester process started\n");
	printf("entering loop.\n");
	
#ifdef CPABE_SETUP
	printf("CPABE_setup(0)\n");
	time_s = clock();
	
	cpabe_setup(&pub, &msk);

	time_f = clock();
	dt0 = time_f - time_s;
	printf("CPABE_setup(0): %lu ms\n", (uint32_t)(dt0*1000/CLOCK_SECOND));

	/* CP-ABE Keys */

/*	printf("CPABE_msk_beta: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
		printf("%x ", msk.beta[i]);
	}
	printf("\n");
	printf("CPABE_msk_g-alpha_x: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
		printf("%x ", msk.g_alpha.x[i]);
	}
	printf("\n");
	printf("CPABE_msk_g-alpha_y: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
		printf("%x ", msk.g_alpha.y[i]);
	}
	printf("\n");
	printf("CPABE_pub_g-hat-alpha_r: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
		printf("%x ", pub.g_hat_alpha.r[i]);
	}
	printf("\n");
	printf("CPABE_pub_g-hat-alpha_i: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
		printf("%x ", pub.g_hat_alpha.i[i]);
	}
	printf("\n");
	printf("CPABE_pub_h_x: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
		printf("%x ", pub.h.x[i]);
	}
	printf("\n");
	printf("CPABE_pub_h_y: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
		printf("%x ", pub.h.y[i]);
	}
	printf("\n");*/
#endif
	
	do {

#ifdef CPABE_KEYGEN
		printf("CPABE_keygen(%d)\n", round_index);
		time_s = clock();
		cpabe_keygen(&prv, pub, msk, attributes);

		time_f = clock();
		dt0 = time_f - time_s;
		printf("CPABE_keygen(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));

/*		printf("CPABE_prv_d_x: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", prv.d.x[i]);
		}
		printf("\n");
		printf("CPABE_prv_d_y: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", prv.d.y[i]);
		}
		printf("\n");*/
#endif
#ifdef CPABE_ENCRYPTION		
		printf("CPABE_enc(%d) \n", round_index);
		printf("enc policy: %s\n", policy);
		time_s = clock();
		
		cpabe_enc(&cph, pub, &m, policy); 
		
		time_f = clock();
		dt0 = time_f - time_s;
		printf("CPABE_enc(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));
		
		printf("m.r  (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x %x ", (uint16_t)(m.r[i] >> 16), (uint16_t) m.r[i]);
		}
		printf("\n");
		printf("m.i  (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x %x ", (uint16_t)(m.r[i] >> 16), (uint16_t) m.r[i]);
		}
		printf("\n");
/*		printf("CPABE_cph_cs_r: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", cph.cs.r[i]);
		}
		printf("\n");
		printf("CPABE_cph_cs_i: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", cph.cs.i[i]);
		}
		printf("\n");
		printf("CPABE_cph_c_x: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", cph.c.x[i]);
		}
		printf("\n");
		printf("CPABE_cph_c_y: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", cph.c.y[i]);
		}
		printf("\n");*/
#endif
#ifdef CPABE_DECRYPTION
		printf("CPABE_dec(%d) \n", round_index);
		time_s = clock();
		
		cpabe_dec(pub, prv, cph, &m2); 
		
		time_f = clock();
		dt0 = time_f - time_s;
		printf("CPABE_dec(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));

		printf("m2.r (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x %x ", (uint16_t)(m.r[i] >> 16), (uint16_t) m.r[i]);
		}
		printf("\n");
		printf("m2.i (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x %x ", (uint16_t)(m.r[i] >> 16), (uint16_t) m.r[i]);
		}
		printf("\n");
#endif
						
		// free dynamic memory
printf("1\n");
		cpabe_prv_free(&prv);
printf("2\n");
		cpabe_cph_free(&cph);
printf("3\n");
		
		round_index++;
	} while(round_index < MAX_ROUNDS);

	printf("Done.\n");

	return 0;
}


