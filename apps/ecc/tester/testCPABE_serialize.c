/*
 *  testCPABE.c
 *  
 *
 *  Created by Daniel Jacobi on 03.08.10.
 *  Copyright 2010 TU Darmstadt. All rights reserved.
 *
 */

#include <string.h>
#include <stdio.h>

#include "list.h"
#include "contiki.h"
#include "leds.h"
#include "watchdog.h"
//#include <contiki-lib.h>
//#include <contiki-net.h>

#include <NN.h>
#include <NN2.h>
#include "TP.h"
#include "ECC.h"
#include "cp-abe.h"
extern void * list_index(list_t list, uint8_t index);

#define MAX_ROUNDS 1

static cpabe_pub_t pub;
static cpabe_msk_t msk;
static cpabe_prv_t prv;
static cpabe_cph_t cph;
static uint8_t round_index = 0;
static char * attributes[8];
static char * policy;
static NN2_NUMBER m;
static NN2_NUMBER m2;
static unsigned long energy_cpu = 0;
static unsigned long energy_lpm = 0;

/* un-/serialize */
static uint8_t serialized[400];

/* declaration of scopes process */
PROCESS(tester_process, "CP-ABE tester process");
AUTOSTART_PROCESSES(&tester_process);

/* scopes process */
PROCESS_THREAD(tester_process, ev, data)
{
	PROCESS_BEGIN();
	int16_t i = 0;
    uint32_t time_s, time_f, dt0;

	watchdog_stop();
	memset((uint8_t *) serialized, 0, 400);
	
//	for (i = 0; i < NUMWORDS-1; i++) {
//		m[i] = i;
//	}
	
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
	/* large */ // Not enough RAM on our MSP430 for this to test.
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
	printf("Envar size: %d\n", sizeof(energy_cpu));
	printf("pub: %d\n", sizeof(cpabe_pub_t));
	printf("msk: %d\n", sizeof(cpabe_msk_t));
	printf("prv: %d\n", sizeof(cpabe_prv_t));
	printf("cph: %d\n", sizeof(cpabe_cph_t));
	printf("comp: %d\n", sizeof(cpabe_prv_comp_t));
	printf("policy: %d\n", sizeof(cpabe_policy_t));
	printf("poly: %d\n", sizeof(cpabe_polynomial_t));
	printf("numwords: %d\n", NUMWORDS);	
	
	/* create and start an event timer */
	static struct etimer tester_timer;
	etimer_set(&tester_timer, 5 * CLOCK_SECOND);
	
	cpabe_init();
	printf("entering loop.\n");
	
	/* wait till the timer expires and then reset it immediately */
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&tester_timer));
//	etimer_reset(&tester_timer);

	do {
#ifdef CPABE_SETUP
	printf("\nCPABE_setup(%d)\n", round_index);
	mem_free_count = 0; memb_comp_free_count = 0; memb_policy_free_count = 0; memb_poly_free_count = 0;
	mem_count = 0; memb_comp_count = 0; memb_policy_count = 0; memb_poly_count = 0;
	energy_lpm = energest_type_time(ENERGEST_TYPE_LPM);
	energy_cpu = energest_type_time(ENERGEST_TYPE_CPU); printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
	time_s = clock_time();
	
	cpabe_setup(&pub, &msk);

	energy_lpm = energest_type_time(ENERGEST_TYPE_LPM) - energy_lpm;
	energy_cpu = energest_type_time(ENERGEST_TYPE_CPU) - energy_cpu;
	time_f = clock_time();
	dt0 = time_f - time_s; printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
	printf("CPABE_setup(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));
	printf("CPABE_setup(%d): dynmem %lu memb_comp %lu memb_policy %lu memb_poly %lu\n", round_index, mem_count, memb_comp_count, memb_policy_count, memb_poly_count);
	printf("CPABE_setup(%d): ENERGEST cpu: %lu lpm: %lu\n", round_index, energy_cpu, energy_lpm);

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
	
		// (un-)serialize keys and output them.
		printf("m1\n");
		uint8_t* ser = cpabe_pub_serialize( &pub );
		printf("m2\n");
		uint8_t* ser_m = cpabe_msk_serialize( &msk );
		printf("m3\n");
		
		printf("CPABE_pub: ");
		for (i = 0; i < (8 * sizeof(NN_DIGIT) * NUMWORDS); i++) {
			printf("0x%x, ", ser[i]);
		}
		printf("\n");
		
		
		cpabe_pub_unserialize( &pub, ser, 1 ); // FIXME: is free still needed
		printf("m4\n");
		cpabe_msk_unserialize( &msk, ser_m, 1 );		
		printf("m5\n");
		
		free(ser);
		printf("m6\n");
		free(ser_m);
		printf("m7\n");
		
		
		
/*		printf("CPABE_msk_beta: ");
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
		printf("\n");
*/		
		
		
		// Pairing
		leds_off(LEDS_GREEN);
		leds_on(LEDS_RED);

		// run tests

#ifdef CPABE_KEYGEN
		printf("CPABE_keygen(%d)\n", round_index);
		mem_count = 0; memb_comp_count = 0; memb_policy_count = 0; memb_poly_count = 0;
		energy_lpm = energest_type_time(ENERGEST_TYPE_LPM);
		energy_cpu = energest_type_time(ENERGEST_TYPE_CPU); printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
		time_s = clock_time();

		cpabe_keygen(&prv, &pub, &msk, attributes);

		energy_lpm = energest_type_time(ENERGEST_TYPE_LPM) - energy_lpm;
		energy_cpu = energest_type_time(ENERGEST_TYPE_CPU) - energy_cpu;
		time_f = clock_time();
		dt0 = time_f - time_s; printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
		printf("CPABE_keygen(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));
		printf("CPABE_keygen(%d): dynmem %lu memb_comp %lu memb_policy %lu memb_poly %lu\n", round_index, mem_count, memb_comp_count, memb_policy_count, memb_poly_count);
		printf("CPABE_keygen(%d): ENERGEST cpu: %lu lpm: %lu\n", round_index, energy_cpu, energy_lpm);

		printf("CPABE_prv_d_x: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", prv.d.x[i]);
		}
		printf("\n");
		printf("CPABE_prv_d_y: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", prv.d.y[i]);
		}
		printf("\n");
		uint16_t list_len = list_length(prv.comps);
		int j;
		for( j = 0; j < list_len; j++ )
		{
			printf("CPABE_comps[%d]_attr: %s\n", j, ((cpabe_prv_comp_t*) list_index(prv.comps, j))->attr);
			printf("CPABE_comps[%d]_d_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->d.x[i]);
			}
			printf("\n");
			printf("CPABE_comps[%d]_d_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->d.y[i]);
			}
			printf("\n");
			printf("CPABE_comps[%d]_dp_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->dp.x[i]);
			}
			printf("\n");
			printf("CPABE_comps[%d]_dp_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->dp.y[i]);
			}
			printf("\n");
		}
		
		
		// (un-)serialize keys and output them.
printf("p1\n");
		cpabe_prv_serialize( serialized, &prv );
		printf("CPABE_prv: ");
		for (i = 0; i < 400; i++) {
			printf("0x%x, ", serialized[i]);
		}
		printf("\n");
		
		
printf("p2\n");
		cpabe_prv_free( &prv );
printf("p3\n");
		cpabe_prv_unserialize( &prv, serialized, 1 ); 
printf("p4\n");
		
		
		/*printf("CPABE_prv_d_x: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", prv.d.x[i]);
		}
		printf("\n");
		printf("CPABE_prv_d_y: ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", prv.d.y[i]);
		}
		printf("\n");
		list_len = list_length(prv.comps);
		for( j = 0; j < list_len; j++ )
		{
			printf("CPABE_comps[%d]_attr: %s\n", j, ((cpabe_prv_comp_t*) list_index(prv.comps, j))->attr);
			printf("CPABE_comps[%d]_d_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->d.x[i]);
			}
			printf("\n");
			printf("CPABE_comps[%d]_d_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->d.y[i]);
			}
			printf("\n");
			printf("CPABE_comps[%d]_dp_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->dp.x[i]);
			}
			printf("\n");
			printf("CPABE_comps[%d]_dp_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_prv_comp_t*) list_index(prv.comps, j))->dp.y[i]);
			}
			printf("\n");
		}*/
		
#endif
#ifdef CPABE_ENCRYPTION
		printf("CPABE_enc(%d) \n", round_index);
		printf("enc policy: %s\n", policy);
		mem_count = 0; memb_comp_count = 0; memb_policy_count = 0; memb_poly_count = 0;
		energy_lpm = energest_type_time(ENERGEST_TYPE_LPM);
		energy_cpu = energest_type_time(ENERGEST_TYPE_CPU); printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
		time_s = clock_time();
		
		cpabe_enc(&cph, &pub, &m, policy); 
		
		energy_lpm = energest_type_time(ENERGEST_TYPE_LPM) - energy_lpm;
		energy_cpu = energest_type_time(ENERGEST_TYPE_CPU) - energy_cpu;
		time_f = clock_time();
		dt0 = time_f - time_s; printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
		printf("CPABE_enc(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));
		printf("CPABE_enc(%d): dynmem %lu memb_comp %lu memb_policy %lu memb_poly %lu\n", round_index, mem_count, memb_comp_count, memb_policy_count, memb_poly_count);
		printf("CPABE_enc(%d): ENERGEST cpu: %lu lpm: %lu\n", round_index, energy_cpu, energy_lpm);
		
		printf("m.r  (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", m.r[i]);
		}
		printf("\n");
		printf("m.i  (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", m.i[i]);
		}
		printf("\n");

		
		printf("CPABE_cph_cs_r: ");
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
		printf("\n");
		/*uint16_t*/ list_len = list_length(cph.p);
		//int j;
		for( j = 0; j < list_len; j++ )
		{
			printf("CPABE_policy[%d]_k+child#: %d, %d\n", j, ((cpabe_policy_t*) list_index(cph.p, j))->k, list_length(((cpabe_policy_t*) list_index(cph.p, j))->children));
			printf("CPABE_policy[%d]_attr: %s\n", j, ((cpabe_policy_t*) list_index(cph.p, j))->attr);
			printf("CPABE_policy[%d]_c_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->c.x[i]);
			}
			printf("\n");
			printf("CPABE_policy[%d]_c_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->c.y[i]);
			}
			printf("\n");
			printf("CPABE_policy[%d]_cp_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->cp.x[i]);
			}
			printf("\n");
			printf("CPABE_policy[%d]_cp_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->cp.y[i]);
			}
			printf("\n");
		}
		
		
		// (un-)serialize keys and output them.
printf("c1\n");
		cpabe_cph_serialize( serialized, &cph );
printf("c2\n");
		cpabe_cph_free(&cph);
printf("c3\n");
		cpabe_cph_unserialize( &cph, serialized, 1 ); 
printf("c4\n");
		
		
		printf("CPABE_cph_cs_r: ");
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
		printf("\n");
		list_len = list_length(cph.p);
		for( j = 0; j < list_len; j++ )
		{
			printf("CPABE_policy[%d]_k+child#: %d, %d\n", j, ((cpabe_policy_t*) list_index(cph.p, j))->k, list_length(((cpabe_policy_t*) list_index(cph.p, j))->children));
			printf("CPABE_policy[%d]_attr: %s\n", j, ((cpabe_policy_t*) list_index(cph.p, j))->attr);
			printf("CPABE_policy[%d]_c_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->c.x[i]);
			}
			printf("\n");
			printf("CPABE_policy[%d]_c_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->c.y[i]);
			}
			printf("\n");
			printf("CPABE_policy[%d]_cp_x: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->cp.x[i]);
			}
			printf("\n");
			printf("CPABE_policy[%d]_cp_y: ", j);
			for (i = NUMWORDS-1; i >= 0; i--) {
				printf("%x ", ((cpabe_policy_t*) list_index(cph.p, j))->cp.y[i]);
			}
			printf("\n");
		}
		
		
		
#endif
#ifdef CPABE_DECRYPTION
		printf("CPABE_dec(%d) \n", round_index);
		mem_count = 0; memb_comp_count = 0; memb_policy_count = 0; memb_poly_count = 0;
		energy_lpm = energest_type_time(ENERGEST_TYPE_LPM);
		energy_cpu = energest_type_time(ENERGEST_TYPE_CPU); printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
		time_s = clock_time();
		
		cpabe_dec(&pub, &prv, &cph, &m2); 
		
		energy_lpm = energest_type_time(ENERGEST_TYPE_LPM) - energy_lpm;
		energy_cpu = energest_type_time(ENERGEST_TYPE_CPU) - energy_cpu;
		time_f = clock_time();
		dt0 = time_f - time_s; printf("cpu: %lu\n", energest_type_time(ENERGEST_TYPE_CPU));
		printf("CPABE_dec(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));
		printf("CPABE_dec(%d): dynmem %lu memb_comp %lu memb_policy %lu memb_poly %lu\n", round_index, mem_count, memb_comp_count, memb_policy_count, memb_poly_count);
		printf("CPABE_dec(%d): ENERGEST cpu: %lu lpm: %lu\n", round_index, energy_cpu, energy_lpm);

		printf("m2.r (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", m2.r[i]);
		}
		printf("\n");
		printf("m2.i (plain): ");
		for (i = NUMWORDS-1; i >= 0; i--) {
			printf("%x ", m2.i[i]);
		}
		printf("\n");
#endif
		
		leds_on(LEDS_GREEN);
		leds_off(LEDS_RED);
				
		// free dynamic memory
		cpabe_prv_free(&prv);
		cpabe_cph_free(&cph);
		
		printf("CPABE_free(%d): dynmem %lu memb_comp %lu memb_policy %lu memb_poly %lu\n", round_index, mem_free_count, memb_comp_free_count, memb_policy_free_count, memb_poly_free_count);

		round_index++;
	} while(round_index < MAX_ROUNDS);

	leds_off(LEDS_GREEN);
	leds_off(LEDS_RED);

	printf("Done.\n");
	PROCESS_END();
}


