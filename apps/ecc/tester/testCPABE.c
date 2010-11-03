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

#define MAX_ROUNDS 10

static cpabe_pub_t pub;
static cpabe_msk_t msk;
static cpabe_prv_t prv;
static uint8_t round_index = 0;


/* declaration of scopes process */
PROCESS(tester_process, "CP-ABE tester process");
AUTOSTART_PROCESSES(&tester_process);

/* scopes process */
PROCESS_THREAD(tester_process, ev, data)
{
	PROCESS_BEGIN();
	uint16_t i = 0;
    uint32_t time_s, time_f, dt0;
	char **attributes = {"attr1", "val1", 
							"attr2", "val2",
							""};

	printf("CP-ABE tester process started\n");
	
	/* create and start an event timer */
	static struct etimer tester_timer;
	etimer_set(&tester_timer, 5 * CLOCK_SECOND);
	
	
	printf("entering loop.\n");
	
	/* wait till the timer expires and then reset it immediately */
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&tester_timer));
	//		etimer_reset(&tester_timer);
	
	printf("CPABE_setup(0)\n");
	time_s = clock_time();
	
	cpabe_setup(&pub, &msk);

	time_f = clock_time();
	dt0 = time_f - time_s;
	printf("CPABE_setup(0): %lu ms\n", (uint32_t)(dt0*1000/CLOCK_SECOND));
	
	/* CP-ABE Keys */
	
	printf("CPABE_msk_beta: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", msk.beta[i]);
	}
	printf("\n");
	printf("CPABE_msk_g-alpha_x: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", msk.g_alpha.x[i]);
	}
	printf("\n");
	printf("CPABE_msk_g-alpha_y: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", msk.g_alpha.y[i]);
	}
	printf("\n");
	printf("CPABE_pub_g-hat-alpha: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", pub.g_hat_alpha[i]);
	}
	printf("\n");
	printf("CPABE_pub_h_x: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", pub.h.x[i]);
	}
	printf("\n");
	printf("CPABE_pub_h_y: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", pub.h.y[i]);
	}
	printf("\n");
	
	
	do {
		// Pairing
		leds_off(LEDS_GREEN);
		leds_on(LEDS_RED);

		// TODO: run tests

		printf("CPABE_keygen(%d)\n", round_index);
		time_s = clock_time();

		cpabe_keygen(&prv, pub, msk, attributes);

		time_f = clock_time();
		dt0 = time_f - time_s;
		printf("CPABE_keygen(%d): %lu ms\n", round_index, (uint32_t)(dt0*1000/CLOCK_SECOND));

		printf("CPABE_prv_d_x: ");
		for (i = 0; i < NUMWORDS; i++) {
			printf("%x ", prv.d.x[i]);
		}
		printf("\n");
		printf("CPABE_prv_d_y: ");
		for (i = 0; i < NUMWORDS; i++) {
			printf("%x ", prv.d.y[i]);
		}
		printf("\n");
		
		leds_on(LEDS_GREEN);
		leds_off(LEDS_RED);
		leds_off(LEDS_GREEN);
				
		round_index++;
	} while(round_index < MAX_ROUNDS);
	
	printf("Done.\n");
	PROCESS_END();
}

