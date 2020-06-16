#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "leds.h"
#include "watchdog.h"

#include "ECC.h"

static void check_clock(){
    uint32_t time_a, time_b, t;
    uint32_t dM_len = 80000uL;

	printf("check_clock(): start stopwatch...\n");
	
    time_a = clock_time();
	
    for (; dM_len > 0; dM_len--) {
		//		asm("nop");
		if ((dM_len / 1000) % 2) {
			leds_on(LEDS_BLUE);
			watchdog_periodic();
		} else {
			leds_off(LEDS_BLUE);
		}
		
	} 
    
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("check_clock(): stopped at %d ms (diff: %d)\n", (uint)(t*1000/CLOCK_SECOND), (uint)t);
	
}



/* declaration of scopes process */
PROCESS(tester_process, "ECC tester process");
AUTOSTART_PROCESSES(&tester_process);

/* scopes process */
PROCESS_THREAD(tester_process, ev, data)
{
  PROCESS_BEGIN();
	int i = 0;

  printf("ECC tester process started\n");
  printf("BCSCTL1: 0x%x  BCSCTL2: 0x%x  DCOCTL: 0x%x\n", BCSCTL1, BCSCTL2, DCOCTL);

  /* create and start an event timer */
  static struct etimer tester_timer;
  etimer_set(&tester_timer, 5 * CLOCK_SECOND);
  
	printf("entering loop.\n");

  do {
    /* wait till the timer expires and then reset it immediately */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&tester_timer));
//    etimer_reset(&tester_timer);

	  printf("ECC_init()\n");
	  
	  ECC_init();
	  
	  
	  /* ECC params */
	  printf("ECC_get_params()\n");

	  Params * p = ECC_get_param();
	  
	  printf("ECC_params: ");
	  for (i = 0; i < NUMWORDS; i++) {
		  printf("%x ", p->p[i]);
  
	  }
	  
	  
	  /* Private key */
	  printf("\nECC_gen_private_key()\n");

	  NN_DIGIT pkey[NUMWORDS];
	  ECC_gen_private_key(pkey);

	  printf("ECC_privkey: ");
	  for (i = 0; i < NUMWORDS; i++) {
		  printf("%x ", pkey[i]);
		  
	  }
	  printf("\n");
	  
	  
	  /* Public key */
	  printf("\nECC_gen_public_key()\n");
	  
	  Point pubkey[2*NUMWORDS];
	  ECC_gen_public_key(pubkey, pkey);
	  
	  printf("ECC_pubkey.x: ");
	  for (i = 0; i < NUMWORDS; i++) {
		  printf("%x ", pubkey->x[i]);
		  
	  }
	  printf("\n");
	  printf("ECC_pubkey.y: ");
	  for (i = 0; i < NUMWORDS; i++) {
		  printf("%x ", pubkey->y[i]);
		  
	  }
	  printf("\n");
	  
	  
	  check_clock();
	  
  } while(1);

  PROCESS_END();
}
