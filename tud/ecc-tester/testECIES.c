#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "watchdog.h"
//#include <contiki-lib.h>
//#include <contiki-net.h>
//#include <net/rime/ctimer.h>

#include "ECC.h"
#include "NN.h"
#include "ECIES.h"
#include "lib/rand.h"

#ifdef TEST_VECTOR
#define MSG_LEN 20
#else
#define MSG_LEN 40
#endif


static Point PublicKey;
static NN_DIGIT PrivateKey[NUMWORDS];
static uint32_t t = 0;
static uint8_t M[MAX_M_LEN];
static int M_len;
static uint8_t Ct[2*KEYDIGITS*NN_DIGIT_LEN + 1 + MAX_M_LEN + HMAC_LEN];
static int C_len;
static uint8_t dM[MAX_M_LEN];

//generate message and init ecc module
static void init_data(){
    int i;
    uint32_t time_a, time_b;
	
		
    //init private key
    memset(PrivateKey, 0, NUMWORDS*NN_DIGIT_LEN);
    //init public key
    memset(PublicKey.x, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(PublicKey.y, 0, NUMWORDS*NN_DIGIT_LEN);
	
#ifdef TEST_VECTOR
    M[0] = 0x61;
    M[1] = 0x62;
    M[2] = 0x63;
    M[3] = 0x64;
    M[4] = 0x65;
    M[5] = 0x66;
    M[6] = 0x67;
    M[7] = 0x68;
    M[8] = 0x69;
    M[9] = 0x6A;
    M[10] = 0x6B;
    M[11] = 0x6C;
    M[12] = 0x6D;
    M[13] = 0x6E;
    M[14] = 0x6F;
    M[15] = 0x70;
    M[16] = 0x71;
    M[17] = 0x72;
    M[18] = 0x73;
    M[19] = 0x74;
    M_len = 20;
#else
    for (i=0; i<MAX_M_LEN; i++){
		M[i] = rand();
    }
    M_len = MSG_LEN;
#endif
	
    time_a = clock_time();
	
    ECC_init();
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECC_init(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
	printf("Init Message: ");
	for (i = 0; i < MSG_LEN; i++) {
		printf("%x ", M[i]);
	}
	printf("\n");
	
}

//generate private key
static void V_PrivateKey(){
	int i;

#ifdef TEST_VECTOR  //TEST_VECTOR
	
#ifdef EIGHT_BIT_PROCESSOR
    PrivateKey[20] = 0x0;
    PrivateKey[19] = 0x45;
    PrivateKey[18] = 0xFB;
    PrivateKey[17] = 0x58;
    PrivateKey[16] = 0xA9;
    PrivateKey[15] = 0x2A;
    PrivateKey[14] = 0x17;
    PrivateKey[13] = 0xAD;
    PrivateKey[12] = 0x4B;
    PrivateKey[11] = 0x15;
    PrivateKey[10] = 0x10;
    PrivateKey[9] = 0x1C;
    PrivateKey[8] = 0x66;
    PrivateKey[7] = 0xE7;
    PrivateKey[6] = 0x4F;
    PrivateKey[5] = 0x27;
    PrivateKey[4] = 0x7E;
    PrivateKey[3] = 0x2B;
    PrivateKey[2] = 0x46;
    PrivateKey[1] = 0x08;
    PrivateKey[0] = 0x66;
#elif defined(SIXTEEN_BIT_PROCESSOR)
    PrivateKey[10] = 0x0;
    PrivateKey[9] = 0x45FB;
    PrivateKey[8] = 0x58A9;
    PrivateKey[7] = 0x2A17;
    PrivateKey[6] = 0xAD4B;
    PrivateKey[5] = 0x1510;
    PrivateKey[4] = 0x1C66;
    PrivateKey[3] = 0xE74F;
    PrivateKey[2] = 0x277E;
    PrivateKey[1] = 0x2B46;
    PrivateKey[0] = 0x0866;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
    PrivateKey[5] = 0x0;
    PrivateKey[4] = 0x45FB58A9;
    PrivateKey[3] = 0x2A17AD4B;
    PrivateKey[2] = 0x15101C66;
    PrivateKey[1] = 0xE74F277E;
    PrivateKey[0] = 0x2B460866;
#endif
	
	
#else//random PrivateKey
    ECC_gen_private_key(PrivateKey);
#endif  //end of test vector
	
	printf("ECC_privkey: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PrivateKey[i]);
		
	}
	printf("\n");
	
}


static void V_PublicKey(){
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


static void U_encrypt(){
    uint32_t time_a, time_b;
	int i;

    time_a = clock_time();
	
    C_len = ECIES_encrypt(Ct, 2*KEYDIGITS*NN_DIGIT_LEN + 1 + M_len + HMAC_LEN, M, M_len, &PublicKey); 
    
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECIES_encrypt(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
	printf("Cyphertext: ");
	for (i = 0; i < C_len; i++) {
		printf("%x ", Ct[i]);
	}
	printf("\n");	

}


static void V_decrypt(){
    uint32_t time_a, time_b;
    int dM_len = MAX_M_LEN;
    int i;
	
	
    time_a = clock_time();
	
    dM_len = ECIES_decrypt(dM, dM_len, Ct, C_len, PrivateKey); 
    
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECIES_decrypt(): %d ms\n", (uint)(t*1000/CLOCK_SECOND));
	printf("dMessage: ");
	for (i = 0; i < dM_len; i++) {
		printf("%x ", dM[i]);
	}
	printf("\n");
	
}


/* declaration of scopes process */
PROCESS(tester_process, "ECIES tester process");
AUTOSTART_PROCESSES(&tester_process);

/* scopes process */
PROCESS_THREAD(tester_process, ev, data)
{
  PROCESS_BEGIN();

  printf("ECIES tester process started\n");

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

	  V_PrivateKey();
	  V_PublicKey();

	  U_encrypt();
	  
	  V_decrypt();
	  
	  
  } while(1);

  PROCESS_END();
}
