#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "leds.h"
#include "watchdog.h"
#include "TP.h"

#define MAX_ROUNDS 10

  
static Point Q;
static NN_DIGIT res[NUMWORDS];
static NN2_NUMBER f;
static int round_index;
static TPParams tp;

static void print_val(NN_DIGIT *num){
    int i;
    for(i=0; i<NUMWORDS; i++){
      printf("%x ", num[NUMWORDS-i-1]);
    }
    printf("\n");
  }

  
static void get_PublicKey() {
#ifdef EIGHT_BIT_PROCESSOR
#ifdef SS192K2
	  Q.x[24]=0x00;
	  Q.x[23]=0xb2;
	  Q.x[22]=0x42;
	  Q.x[21]=0x1f;
	  Q.x[20]=0xda;
	  Q.x[19]=0x74;
	  Q.x[18]=0x4b;
	  Q.x[17]=0xb1;
	  Q.x[16]=0x79;
	  Q.x[15]=0x20;
	  Q.x[14]=0x2d;
	  Q.x[13]=0x4d;
	  Q.x[12]=0x5b;
	  Q.x[11]=0xb0;
	  Q.x[10]=0x9b;
	  Q.x[9]=0x5a;
	  Q.x[8]=0x1f;
	  Q.x[7]=0x8b;
	  Q.x[6]=0x18;
	  Q.x[5]=0x4e;
	  Q.x[4]=0xb7;
	  Q.x[3]=0xe7;
	  Q.x[2]=0x61;
	  Q.x[1]=0x67;
	  Q.x[0]=0xc6;
	  
	  Q.y[24]=0x00;
	  Q.y[23]=0xdd;
	  Q.y[22]=0xa6;
	  Q.y[21]=0xa9;
	  Q.y[20]=0x30;
	  Q.y[19]=0x31;
	  Q.y[18]=0xc5;
	  Q.y[17]=0x33;
	  Q.y[16]=0x41;
	  Q.y[15]=0xec;
	  Q.y[14]=0xf6;
	  Q.y[13]=0xee;
	  Q.y[12]=0x19;
	  Q.y[11]=0x6a;
	  Q.y[10]=0x6f;
	  Q.y[9]=0xdd;
	  Q.y[8]=0xa9;
	  Q.y[7]=0x81;
	  Q.y[6]=0xa1;
	  Q.y[5]=0xdb;
	  Q.y[4]=0xd3;
	  Q.y[3]=0xcc;
	  Q.y[2]=0xa8;
	  Q.y[1]=0x65;
	  Q.y[0]=0x96;
	  
	  
	  
#elif defined(SS512K2)
    Q.x[64]=0x00;
    Q.x[63]=0x0B;
    Q.x[62]=0x53;
    Q.x[61]=0x48;
    Q.x[60]=0x39;
    Q.x[59]=0x95;
    Q.x[58]=0x7A;
    Q.x[57]=0xD9;
    Q.x[56]=0x0C;
    Q.x[55]=0x6C;
    Q.x[54]=0xD3;
    Q.x[53]=0xA7;
    Q.x[52]=0x1A;
    Q.x[51]=0x36;
    Q.x[50]=0x4A;
    Q.x[49]=0x03;
    Q.x[48]=0x13;
    Q.x[47]=0x58;
    Q.x[46]=0xA1;
    Q.x[45]=0x3D;
    Q.x[44]=0x0B;
    Q.x[43]=0x4F;
    Q.x[42]=0x26;
    Q.x[41]=0xDD;
    Q.x[40]=0x26;
    Q.x[39]=0xF5;
    Q.x[38]=0xBA;
    Q.x[37]=0xB6;
    Q.x[36]=0xA8;
    Q.x[35]=0x52;
    Q.x[34]=0xF4;
    Q.x[33]=0xCE;
    Q.x[32]=0x8C;
    Q.x[31]=0x92;
    Q.x[30]=0xD8;
    Q.x[29]=0xC8;
    Q.x[28]=0x4E;
    Q.x[27]=0xEF;
    Q.x[26]=0xE0;
    Q.x[25]=0xA9;
    Q.x[24]=0xF0;
    Q.x[23]=0xDD;
    Q.x[22]=0xBE;
    Q.x[21]=0xE2;
    Q.x[20]=0xEA;
    Q.x[19]=0xBE;
    Q.x[18]=0x70;
    Q.x[17]=0xEF;
    Q.x[16]=0x0E;
    Q.x[15]=0x21;
    Q.x[14]=0xE9;
    Q.x[13]=0x02;
    Q.x[12]=0xFE;
    Q.x[11]=0x27;
    Q.x[10]=0x59;
    Q.x[9]=0x5F;
    Q.x[8]=0x56;
    Q.x[7]=0xB7;
    Q.x[6]=0xFA;
    Q.x[5]=0x10;
    Q.x[4]=0xB8;
    Q.x[3]=0xFB;
    Q.x[2]=0x97;
    Q.x[1]=0x76;
    Q.x[0]=0x9D;
    

    Q.y[64]=0x00;
    Q.y[63]=0x70;
    Q.y[62]=0x32;
    Q.y[61]=0xE2;
    Q.y[60]=0x62;
    Q.y[59]=0xB0;
    Q.y[58]=0x68;
    Q.y[57]=0x12;
    Q.y[56]=0x93;
    Q.y[55]=0x5C;
    Q.y[54]=0xFE;
    Q.y[53]=0x57;
    Q.y[52]=0x3F;
    Q.y[51]=0xC8;
    Q.y[50]=0x5A;
    Q.y[49]=0x98;
    Q.y[48]=0xE2;
    Q.y[47]=0xBC;
    Q.y[46]=0xA9;
    Q.y[45]=0xC1;
    Q.y[44]=0x7F;
    Q.y[43]=0x85;
    Q.y[42]=0x77;
    Q.y[41]=0x97;
    Q.y[40]=0x3D;
    Q.y[39]=0xAA;
    Q.y[38]=0xCE;
    Q.y[37]=0xA5;
    Q.y[36]=0xF2;
    Q.y[35]=0xE7;
    Q.y[34]=0xCA;
    Q.y[33]=0xAD;
    Q.y[32]=0xB2;
    Q.y[31]=0xA3;
    Q.y[30]=0xCC;
    Q.y[29]=0x42;
    Q.y[28]=0x94;
    Q.y[27]=0xA4;
    Q.y[26]=0x10;
    Q.y[25]=0x9A;
    Q.y[24]=0x41;
    Q.y[23]=0x25;
    Q.y[22]=0x0E;
    Q.y[21]=0x2E;
    Q.y[20]=0x72;
    Q.y[19]=0x25;
    Q.y[18]=0x18;
    Q.y[17]=0x90;
    Q.y[16]=0xAB;
    Q.y[15]=0x2E;
    Q.y[14]=0xCC;
    Q.y[13]=0x9D;
    Q.y[12]=0x35;
    Q.y[11]=0xD4;
    Q.y[10]=0xFF;
    Q.y[9]=0x88;
    Q.y[8]=0x7D;
    Q.y[7]=0x44;
    Q.y[6]=0x01;
    Q.y[5]=0x41;
    Q.y[4]=0xFD;
    Q.y[3]=0xD0;
    Q.y[2]=0x8F;
    Q.y[1]=0x1B;
    Q.y[0]=0x61;
#endif
#elif defined(SIXTEEN_BIT_PROCESSOR)
#ifdef SS192K2
	  
	  Q.x[12]=0x0000;
	  Q.x[11]=0xb242;
	  Q.x[10]=0x1fda;
	  Q.x[9]=0x744b;
	  Q.x[8]=0xb179;
	  Q.x[7]=0x202d;
	  Q.x[6]=0x4d5b;
	  Q.x[5]=0xb09b;
	  Q.x[4]=0x5a1f;
	  Q.x[3]=0x8b18;
	  Q.x[2]=0x4eb7;
	  Q.x[1]=0xe761;
	  Q.x[0]=0x67c6;
	  
	  Q.y[12]=0x0000;
	  Q.y[11]=0xdda6;
	  Q.y[10]=0xa930;
	  Q.y[9]=0x31c5;
	  Q.y[8]=0x3341;
	  Q.y[7]=0xecf6;
	  Q.y[6]=0xee19;
	  Q.y[5]=0x6a6f;
	  Q.y[4]=0xdda9;
	  Q.y[3]=0x81a1;
	  Q.y[2]=0xdbd3;
	  Q.y[1]=0xcca8;
	  Q.y[0]=0x6596;
	  
#elif defined(SS512K2)
	  
	  Q.x[32]=0x0000;
	  Q.x[31]=0x0B53;
	  Q.x[30]=0x4839;
	  Q.x[29]=0x957A;
	  Q.x[28]=0xD90C;
	  Q.x[27]=0x6CD3;
	  Q.x[26]=0xA71A;
	  Q.x[25]=0x364A;
	  Q.x[24]=0x0313;
	  Q.x[23]=0x58A1;
	  Q.x[22]=0x3D0B;
	  Q.x[21]=0x4F26;
	  Q.x[20]=0xDD26;
	  Q.x[19]=0xF5BA;
	  Q.x[18]=0xB6A8;
	  Q.x[17]=0x52F4;
	  Q.x[16]=0xCE8C;
	  Q.x[15]=0x92D8;
	  Q.x[14]=0xC84E;
	  Q.x[13]=0xEFE0;
	  Q.x[12]=0xA9F0;
	  Q.x[11]=0xDDBE;
	  Q.x[10]=0xE2EA;
	  Q.x[9]=0xBE70;
	  Q.x[8]=0xEF0E;
	  Q.x[7]=0x21E9;
	  Q.x[6]=0x02FE;
	  Q.x[5]=0x2759;
	  Q.x[4]=0x5F56;
	  Q.x[3]=0xB7FA;
	  Q.x[2]=0x10B8;
	  Q.x[1]=0xFB97;
	  Q.x[0]=0x769D;
	  
	  Q.y[32]=0x0000;
	  Q.y[31]=0x7032;
	  Q.y[30]=0xE262;
	  Q.y[29]=0xB068;
	  Q.y[28]=0x1293;
	  Q.y[27]=0x5CFE;
	  Q.y[26]=0x573F;
	  Q.y[25]=0xC85A;
	  Q.y[24]=0x98E2;
	  Q.y[23]=0xBCA9;
	  Q.y[22]=0xC17F;
	  Q.y[21]=0x8577;
	  Q.y[20]=0x973D;
	  Q.y[19]=0xAACE;
	  Q.y[18]=0xA5F2;
	  Q.y[17]=0xE7CA;
	  Q.y[16]=0xADB2;
	  Q.y[15]=0xA3CC;
	  Q.y[14]=0x4294;
	  Q.y[13]=0xA410;
	  Q.y[12]=0x9A41;
	  Q.y[11]=0x250E;
	  Q.y[10]=0x2E72;
	  Q.y[9]=0x2518;
	  Q.y[8]=0x90AB;
	  Q.y[7]=0x2ECC;
	  Q.y[6]=0x9D35;
	  Q.y[5]=0xD4FF;
	  Q.y[4]=0x887D;
	  Q.y[3]=0x4401;
	  Q.y[2]=0x41FD;
	  Q.y[1]=0xD08F;
	  Q.y[0]=0x1B61;
	  
#endif // CURVE 
	  
	  
#elif defined(THIRTYTWO_BIT_PROCESSOR)

    #ifdef SS192K2
    
    Q.x[6]=0x00000000;
    Q.x[5]=0xb2421fda;
    Q.x[4]=0x744bb179;
    Q.x[3]=0x202d4d5b;
    Q.x[2]=0xb09b5a1f;
    Q.x[1]=0x8b184eb7;
    Q.x[0]=0xe76167c6;
        
    Q.y[6]=0x00000000;
    Q.y[5]=0xdda6a930;
    Q.y[4]=0x31c53341;
    Q.y[3]=0xecf6ee19;
    Q.y[2]=0x6a6fdda9;
    Q.y[1]=0x81a1dbd3;
    Q.y[0]=0xcca86596;
    
    #elif defined(SS512K2)
    
    Q.x[16]=0x00000000;
    Q.x[15]=0x0B534839;
    Q.x[14]=0x957AD90C;
    Q.x[13]=0x6CD3A71A;
    Q.x[12]=0x364A0313;
    Q.x[11]=0x58A13D0B;
    Q.x[10]=0x4F26DD26;
    Q.x[9]=0xF5BAB6A8;
    Q.x[8]=0x52F4CE8C;
    Q.x[7]=0x92D8C84E;
    Q.x[6]=0xEFE0A9F0;
    Q.x[5]=0xDDBEE2EA;
    Q.x[4]=0xBE70EF0E;
    Q.x[3]=0x21E902FE;
    Q.x[2]=0x27595F56;
    Q.x[1]=0xB7FA10B8;
    Q.x[0]=0xFB97769D;
    
    Q.y[16]=0x00000000;
    Q.y[15]=0x7032E262;
    Q.y[14]=0xB0681293;
    Q.y[13]=0x5CFE573F;
    Q.y[12]=0xC85A98E2;
    Q.y[11]=0xBCA9C17F;
    Q.y[10]=0x8577973D;
    Q.y[9]=0xAACEA5F2;
    Q.y[8]=0xE7CAADB2;
    Q.y[7]=0xA3CC4294;
    Q.y[6]=0xA4109A41;
    Q.y[5]=0x250E2E72;
    Q.y[4]=0x251890AB;
    Q.y[3]=0x2ECC9D35;
    Q.y[2]=0xD4FF887D;
    Q.y[1]=0x440141FD;
    Q.y[0]=0xD08F1B61;
    
    #endif // CURVE 
    #endif // PROC  
  }

static void runTP(){
    
    uint32_t time_s, time_f, dt0, dt1, dt2;
	get_TP_param(&tp);
    
	leds_toggle(LEDS_BLUE);
    get_PublicKey();

    while (round_index<MAX_ROUNDS) {
		leds_toggle(LEDS_BLUE);
		time_s = clock_time();
		
		TP_init(tp.P, Q);
//		TP_init(Q);
		
		time_f = clock_time();
		leds_toggle(LEDS_BLUE);
		
		dt0 = time_f - time_s;
		
		time_s = clock_time();
		
		TP_Miller(&f, tp.P);
//		TP_Miller(&f);
		
		time_f = clock_time();
		leds_toggle(LEDS_BLUE);
		
		dt1 = time_f - time_s;
		
 	  time_s = clock_time();

		TP_final_expon(res,&f);

	  time_f = clock_time();
		leds_toggle(LEDS_BLUE);

	  dt2 = time_f - time_s;
      
      
      printf("TP= ");
      print_val(res);
      printf("time(%d): init: %lu ms, miller: %lu ms, exp: %lu ms, miller+exp: %lu ms\n", 
			 round_index, (uint32_t)(dt0*1000/CLOCK_SECOND), 
						  (uint32_t)(dt1*1000/CLOCK_SECOND), 
						  (uint32_t)(dt2*1000/CLOCK_SECOND), 
						  (uint32_t)((dt1+dt2)*1000/CLOCK_SECOND));
      

      round_index++;
      
    }
    
  }
  

/* declaration of scopes process */
PROCESS(tester_process, "TP tester process");
AUTOSTART_PROCESSES(&tester_process);

/* scopes process */
PROCESS_THREAD(tester_process, ev, data)
{
	PROCESS_BEGIN();
	
	printf("TP tester process started\n");
watchdog_stop();

	round_index = 0;

	/* create and start an event timer */
	static struct etimer tester_timer;
	etimer_set(&tester_timer, 5 * CLOCK_SECOND);
	
	
	printf("entering loop.\n");
	
	/* wait till the timer expires and then reset it immediately */
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&tester_timer));
//		etimer_reset(&tester_timer);
//	do {
		
		leds_on(LEDS_RED);
		runTP();
		leds_on(LEDS_GREEN);
		leds_off(LEDS_RED);
		leds_off(LEDS_GREEN);
		
//	} while(round_index < MAX_ROUNDS);

  PROCESS_END();
}


