#include <string.h>
#include <stdio.h>

#include "contiki.h"
//#include <contiki-lib.h>
//#include <contiki-net.h>
//#include <net/rime/ctimer.h>

#include "ECC.h"
#include "NN.h"
#include "ECDSA.h"
#include "lib/rand.h"

#ifdef TEST_VECTOR
#define MSG_LEN 3
#else
#define MSG_LEN 52
#endif

static NN_DIGIT PrivateKey[NUMWORDS];
static Point PublicKey;
static uint8_t message[MSG_LEN];
static NN_DIGIT r[NUMWORDS];
static NN_DIGIT s[NUMWORDS];
static uint8_t pass;
static uint32_t t = 0;

static void gen_PrivateKey();

/* declaration of scopes process */
PROCESS(tester_process, "ECDSA tester process");
AUTOSTART_PROCESSES(&tester_process);

static void init_data(){
#ifndef TEST_VECTOR
    uint8_t j;
#endif
		
    //init message
    memset(message, 0, MSG_LEN);
    //init private key
    memset(PrivateKey, 0, NUMWORDS*NN_DIGIT_LEN);
    //init public key
    memset(PublicKey.x, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(PublicKey.y, 0, NUMWORDS*NN_DIGIT_LEN);
    //init signature
    memset(r, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(s, 0, NUMWORDS*NN_DIGIT_LEN);
	
#ifndef TEST_VECTOR  
    //randomly generate the message
    for (j=0; j<MSG_LEN; j++){
		message[j] = (uint8_t) rand();
    }
#else  //only for secp160r1
    message[0] = 0x61;
    message[1] = 0x62;
    message[2] = 0x63;
#endif
    ECC_init();
}

static void gen_PrivateKey(){
	int i;
	
#ifndef TEST_VECTOR  //random private key
	
    /*
	 uint8_t j;
	 
	 for (j=0; j<KEYDIGITS; j++){
	 #ifdef THIRTYTWO_BIT_PROCESSOR
	 PrivateKey[j] = ((uint32_t)call Random.rand() << 16)^((uint32_t)call Random.rand());
	 #else
	 PrivateKey[j] = (NN_DIGIT) call Random.rand();
	 #endif
	 }
	 */
	
    ECC_gen_private_key(PrivateKey);
	
#else  //only for test vector of secp160r1
	
#ifdef EIGHT_BIT_PROCESSOR
    PrivateKey[20] = 0x0;
    PrivateKey[19] = 0xAA;
    PrivateKey[18] = 0x37;
    PrivateKey[17] = 0x4F;
    PrivateKey[16] = 0xFC;
    PrivateKey[15] = 0x3C;
    PrivateKey[14] = 0xE1;
    PrivateKey[13] = 0x44;
    PrivateKey[12] = 0xE6;
    PrivateKey[11] = 0xB0;
    PrivateKey[10] = 0x73;
    PrivateKey[9] = 0x30;
    PrivateKey[8] = 0x79;
    PrivateKey[7] = 0x72;
    PrivateKey[6] = 0xCB;
    PrivateKey[5] = 0x6D;
    PrivateKey[4] = 0x57;
    PrivateKey[3] = 0xB2;
    PrivateKey[2] = 0xA4;
    PrivateKey[1] = 0xE9;
    PrivateKey[0] = 0x82;
#else
#ifdef SIXTEEN_BIT_PROCESSOR
    PrivateKey[10] = 0x0;
    PrivateKey[9] = 0xAA37;
    PrivateKey[8] = 0x4FFC;
    PrivateKey[7] = 0x3CE1;
    PrivateKey[6] = 0x44E6;
    PrivateKey[5] = 0xB073;
    PrivateKey[4] = 0x3079;
    PrivateKey[3] = 0x72CB;
    PrivateKey[2] = 0x6D57;
    PrivateKey[1] = 0xB2A4;
    PrivateKey[0] = 0xE982;
#else
#ifdef THIRTYTWO_BIT_PROCESSOR
    PrivateKey[5] = 0x0;
    PrivateKey[4] = 0xAA374FFC;
    PrivateKey[3] = 0x3CE144E6;
    PrivateKey[2] = 0xB0733079;
    PrivateKey[1] = 0x72CB6D57;
    PrivateKey[0] = 0xB2A4E982;
#endif  //end of 32-bit
#endif  //end of 16-bit
#endif  //end of 8-bit
    
#endif  //end of test vector
    
	printf("ECC_privkey: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PrivateKey[i]);
		
	}
	printf("\n");
}

static void ecc__init(){
    uint32_t time_a, time_b;
		
    time_a = clock_time();
	
    ECC_init();
	
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECC_init(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
}

static void gen_PublicKey(){
    uint32_t time_a, time_b;
	int i;
	
	
    time_a = clock_time();

    ECC_gen_public_key(&PublicKey, PrivateKey);
	
    time_b = clock_time();
    t = time_b - time_a;
	

	printf("ECC_gen_public_key(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
	printf("ECC_pubkey.x: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PublicKey.x[i]);
		
	}
	printf("\n");
	printf("ECC_pubkey.y: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PublicKey.y[i]);
		
	}
	printf("\n");
    
}

static void ecdsa_init(){
    uint32_t time_a, time_b;
	
    time_a = clock_time();
	
    ECDSA_init(&PublicKey); 
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECDSA_init(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
}

static void sign(){
    uint32_t time_a, time_b;
	
    time_a = clock_time();
	
    ECDSA_sign(message, MSG_LEN, r, s, PrivateKey);
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECDSA_sign(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
// TODO output
}

static void verify(){
    uint32_t time_a, time_b;
	
	
    time_a = clock_time();
	
    pass = ECDSA_verify(message, MSG_LEN, r, s, &PublicKey);   
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECDSA_verify(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
	printf("ECDSA_verify(): %s\n", pass?"passed":"not passed");
}


/* scopes process */
PROCESS_THREAD(tester_process, ev, data)
{
  PROCESS_BEGIN();

  printf("ECDSA tester process started\n");

  /* create and start an event timer */
  static struct etimer tester_timer;
  etimer_set(&tester_timer, 5 * CLOCK_SECOND);
	watchdog_stop();
	
	init_data();
	
	printf("entering loop.\n");

  do {
    /* wait till the timer expires and then reset it immediately */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&tester_timer));
//    etimer_reset(&tester_timer);

	  gen_PrivateKey();
	  ecc__init();
	  gen_PublicKey();
	  
	  ecdsa_init();
	  
	  sign();
	  verify();
	  
  } while(1);

  PROCESS_END();
}
