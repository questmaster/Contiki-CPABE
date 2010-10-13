#include <string.h>
#include <stdio.h>

#include "contiki.h"

#include "ECDH.h"


static NN_DIGIT PrivateKey1[NUMWORDS];
static NN_DIGIT PrivateKey2[NUMWORDS];
static Point PublicKey1;
static Point PublicKey2;
static uint8_t Secret1[KEYDIGITS*NN_DIGIT_LEN];
static uint8_t Secret2[KEYDIGITS*NN_DIGIT_LEN];
static uint32_t t = 0;

static int round_index;
#define MAX_ROUNDS 10


/* declaration of scopes process */
PROCESS(tester_process, "ECDH tester process");
AUTOSTART_PROCESSES(&tester_process);

static void init_data(){
    uint32_t time_a, time_b;
	
    //init private key
    memset(PrivateKey1, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(PrivateKey2, 0, NUMWORDS*NN_DIGIT_LEN);
    //init public key
    memset(PublicKey1.x, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(PublicKey1.y, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(PublicKey2.x, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(PublicKey2.y, 0, NUMWORDS*NN_DIGIT_LEN);
	
    time_a = clock_time();
	
    ECDH_init();
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECDH_init(%d): %d ms\n", round_index, (uint)(t*1000/CLOCK_SECOND));
}

void gen_PrivateKey1(){
	int i;
	
#ifdef TEST_VECTOR  //TEST_VECTOR
#ifdef EIGHT_BIT_PROCESSOR
    PrivateKey1[20] = 0x0;
    PrivateKey1[19] = 0xAA;
    PrivateKey1[18] = 0x37;
    PrivateKey1[17] = 0x4F;
    PrivateKey1[16] = 0xFC;
    PrivateKey1[15] = 0x3C;
    PrivateKey1[14] = 0xE1;
    PrivateKey1[13] = 0x44;
    PrivateKey1[12] = 0xE6;
    PrivateKey1[11] = 0xB0;
    PrivateKey1[10] = 0x73;
    PrivateKey1[9] = 0x30;
    PrivateKey1[8] = 0x79;
    PrivateKey1[7] = 0x72;
    PrivateKey1[6] = 0xCB;
    PrivateKey1[5] = 0x6D;
    PrivateKey1[4] = 0x57;
    PrivateKey1[3] = 0xB2;
    PrivateKey1[2] = 0xA4;
    PrivateKey1[1] = 0xE9;
    PrivateKey1[0] = 0x82;
#elif defined(SIXTEEN_BIT_PROCESSOR)
    PrivateKey1[10] = 0x0;
    PrivateKey1[9] = 0xAA37;
    PrivateKey1[8] = 0x4FFC;
    PrivateKey1[7] = 0x3CE1;
    PrivateKey1[6] = 0x44E6;
    PrivateKey1[5] = 0xB073;
    PrivateKey1[4] = 0x3079;
    PrivateKey1[3] = 0x72CB;
    PrivateKey1[2] = 0x6D57;
    PrivateKey1[1] = 0xB2A4;
    PrivateKey1[0] = 0xE982;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
    PrivateKey1[5] = 0x0;
    PrivateKey1[4] = 0xAA374FFC;
    PrivateKey1[3] = 0x3CE144E6;
    PrivateKey1[2] = 0xB0733079;
    PrivateKey1[1] = 0x72CB6D57;
    PrivateKey1[0] = 0xB2A4E982;
#endif
    
#else//random PrivateKey1
    ECC_gen_private_key(PrivateKey1);
#endif  //end of test vector
	printf("ECC_privkey1: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PrivateKey1[i]);
		
	}
	printf("\n");
	
}

void gen_PublicKey1(){
    uint32_t time_a, time_b;
	int i;
	
	
    time_a = clock_time();
	
    ECC_gen_public_key(&PublicKey1, PrivateKey1);
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECC_gen_public_key(%d) 1: %u ms\n", round_index, (uint)(t*1000/CLOCK_SECOND));
	printf("ECC_pubkey1.x: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PublicKey1.x[i]);
		
	}
	printf("\n");
	printf("ECC_pubkey1.y: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PublicKey1.y[i]);
		
	}
	printf("\n");
	
}

void gen_PrivateKey2(){
	int i;
    
#ifdef TEST_VECTOR
#ifdef EIGHT_BIT_PROCESSOR
    PrivateKey2[20] = 0x0;
    PrivateKey2[19] = 0x45;
    PrivateKey2[18] = 0xFB;
    PrivateKey2[17] = 0x58;
    PrivateKey2[16] = 0xA9;
    PrivateKey2[15] = 0x2A;
    PrivateKey2[14] = 0x17;
    PrivateKey2[13] = 0xAD;
    PrivateKey2[12] = 0x4B;
    PrivateKey2[11] = 0x15;
    PrivateKey2[10] = 0x10;
    PrivateKey2[9] = 0x1C;
    PrivateKey2[8] = 0x66;
    PrivateKey2[7] = 0xE7;
    PrivateKey2[6] = 0x4F;
    PrivateKey2[5] = 0x27;
    PrivateKey2[4] = 0x7E; 
    PrivateKey2[3] = 0x2B;
    PrivateKey2[2] = 0x46;
    PrivateKey2[1] = 0x08;
    PrivateKey2[0] = 0x66;
#elif defined(SIXTEEN_BIT_PROCESSOR)
    PrivateKey2[10] = 0x0;
    PrivateKey2[9] = 0x45FB;
    PrivateKey2[8] = 0x58A9;
    PrivateKey2[7] = 0x2A17;
    PrivateKey2[6] = 0xAD4B;
    PrivateKey2[5] = 0x1510;
    PrivateKey2[4] = 0x1C66;
    PrivateKey2[3] = 0xE74F;
    PrivateKey2[2] = 0x277E; 
    PrivateKey2[1] = 0x2B46;
    PrivateKey2[0] = 0x0866;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
    PrivateKey2[5] = 0x0;
    PrivateKey2[4] = 0x45FB58A9;
    PrivateKey2[3] = 0x2A17AD4B;
    PrivateKey2[2] = 0x15101C66;
    PrivateKey2[1] = 0xE74F277E; 
    PrivateKey2[0] = 0x2B460866;
#endif
#else
    ECC_gen_private_key(PrivateKey2);      
#endif
	
	printf("ECC_privkey2: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PrivateKey2[i]);
	}
	printf("\n");
	
}

void gen_PublicKey2(){
    uint32_t time_a, time_b;
	int i;
	
	
    time_a = clock_time();
	
    ECC_gen_public_key(&PublicKey2, PrivateKey2);
	
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECC_gen_public_key(%d) 2: %u ms\n", round_index, (uint)(t*1000/CLOCK_SECOND));
	printf("ECC_pubkey2.x: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PublicKey2.x[i]);
	}
	printf("\n");
	printf("ECC_pubkey2.y: ");
	for (i = 0; i < NUMWORDS; i++) {
		printf("%x ", PublicKey2.y[i]);
	}
	printf("\n");
	
}

void establish1(){
    uint32_t time_a, time_b;
	int i;
	
    time_a = clock_time();
	
    ECDH_key_agree(Secret1, KEYDIGITS*NN_DIGIT_LEN, &PublicKey2, PrivateKey1);
    
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECDH_key_agree(%d) 1: %u ms\n", round_index, (uint)(t*1000/CLOCK_SECOND));
	printf("ECDH_secret1: ");
	for (i = 0; i < KEYDIGITS*NN_DIGIT_LEN; i++) {
		printf("%x ", Secret1[i]);
	}
	printf("\n");
	
}

void establish2(){
    uint32_t time_a, time_b;
	int i;
	
    time_a = clock_time();
	
    ECDH_key_agree(Secret2, KEYDIGITS*NN_DIGIT_LEN, &PublicKey1, PrivateKey2);
    
    time_b = clock_time();
    t = time_b - time_a;
	
	printf("ECDH_key_agree(%d) 2: %u ms\n", round_index, (uint)(t*1000/CLOCK_SECOND));
	printf("ECDH_secret2: ");
	for (i = 0; i < KEYDIGITS*NN_DIGIT_LEN; i++) {
		printf("%x ", Secret2[i]);
	}
	printf("\n");
	
}


/* scopes process */
PROCESS_THREAD(tester_process, ev, data)
{
  PROCESS_BEGIN();

  printf("ECDH tester process started\n");

  /* create and start an event timer */
  static struct etimer tester_timer;
  etimer_set(&tester_timer, 5 * CLOCK_SECOND);
  
	
	printf("entering loop.\n");

    /* wait till the timer expires and then reset it immediately */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&tester_timer));
//    etimer_reset(&tester_timer);
  do {
	  init_data();

	  gen_PrivateKey1();
	  gen_PublicKey1();
	  
	  gen_PrivateKey2();
	  gen_PublicKey2();
	  
	  establish1();
	  establish2();
	  
	  round_index++;
	  
  } while(round_index < MAX_ROUNDS);

  PROCESS_END();
}
