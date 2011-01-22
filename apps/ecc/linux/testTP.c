#include <string.h>
#include <stdio.h>
#include <time.h>

#include "TP.h"

#define MAX_ROUNDS 1//0
#define CLOCK_SECOND CLOCKS_PER_SEC 

  
static Point Q, P;
static NN2_NUMBER res;
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
#if defined(SIXTEEN_BIT_PROCESSOR)
	  
	  Q.x[12]=0x0000;
	  Q.x[11]=0x1caa;
	  Q.x[10]=0xfc3f;
	  Q.x[9]=0x698f;
	  Q.x[8]=0xd00b;
	  Q.x[7]=0x07a5;
	  Q.x[6]=0xff5f;
	  Q.x[5]=0x3a2c;
	  Q.x[4]=0x3645;
	  Q.x[3]=0x41f3;
	  Q.x[2]=0xee91;
	  Q.x[1]=0x3c56;
	  Q.x[0]=0x1648;
	  
	  Q.y[12]=0x0000;
	  Q.y[11]=0x18df;
	  Q.y[10]=0x57e8;
	  Q.y[9]=0x30ea;
	  Q.y[8]=0x8f2d;
	  Q.y[7]=0x8172;
	  Q.y[6]=0xd8d2;
	  Q.y[5]=0x99f1;
	  Q.y[4]=0xc861;
	  Q.y[3]=0xa7f5;
	  Q.y[2]=0x7daa;
	  Q.y[1]=0xfec5;
	  Q.y[0]=0x7e5c;
	  
	  
	P.x[12] = 0x0000;
	P.x[11] = 0x14a7;
	P.x[10] = 0xe9bd;
	P.x[9] = 0xf77a;
	P.x[8] = 0x1e18;
	P.x[7] = 0xd1e1;
	P.x[6] = 0x0e06;
	P.x[5] = 0x6b63;
	P.x[4] = 0x0044;
	P.x[3] = 0x030b;
	P.x[2] = 0x830b;
	P.x[1] = 0x8b37;
	P.x[0] = 0x4869;

	P.y[12] = 0x0000;
	P.y[11] = 0x1492;
	P.y[10] = 0x070c;
	P.y[9] = 0x797c;
	P.y[8] = 0xee4b;
	P.y[7] = 0x75eb;
	P.y[6] = 0x5244;
	P.y[5] = 0xda95;
	P.y[4] = 0xc61b;
	P.y[3] = 0x3caf;
	P.y[2] = 0xd18a;
	P.y[1] = 0xb204;
	P.y[0] = 0xd054;
	  
	  
	  
#elif defined(THIRTYTWO_BIT_PROCESSOR)

    
	  Q.x[6]=0x00000000;
	  Q.x[5]=0x1caafc3f;
	  Q.x[4]=0x698fd00b;
	  Q.x[3]=0x07a5ff5f;
	  Q.x[2]=0x3a2c3645;
	  Q.x[1]=0x41f3ee91;
	  Q.x[0]=0x3c561648;
	  
	  Q.y[6]=0x00000000;
	  Q.y[5]=0x18df57e8;
	  Q.y[4]=0x30ea8f2d;
	  Q.y[3]=0x8172d8d2;
	  Q.y[2]=0x99f1c861;
	  Q.y[1]=0xa7f57daa;
	  Q.y[0]=0xfec57e5c;


    P.x[6]=0x00000000;
    P.x[5]=0x14a7e9bd;
    P.x[4]=0xf77a1e18;
    P.x[3]=0xd1e10e06;
    P.x[2]=0x6b630044;
    P.x[1]=0x030b830b;
    P.x[0]=0x8b374869;
    
    P.y[6]=0x00000000;
    P.y[5]=0x1492070c;
    P.y[4]=0x797cee4b;
    P.y[3]=0x75eb5244;
    P.y[2]=0xda95c61b;
    P.y[1]=0x3cafd18a;
    P.y[0]=0xb204d054;
    
    #endif // PROC  
  }

static void runTP(){
    
    uint32_t time_s, time_f, dt0, dt1, dt2;
    int i;

	get_TP_param(&tp);
    
    get_PublicKey();

    while (round_index<MAX_ROUNDS) {

		time_s = clock();
		
		TP_init(P, Q);
		
		time_f = clock();
		dt0 = time_f - time_s;
		
		time_s = clock();
		
		TP_Miller(&f, P);
/*
    f.r[6]=0x00000000;
    f.r[5]=0x170e061d;
    f.r[4]=0x89508f11;
    f.r[3]=0xec7f11de;
    f.r[2]=0x1583ce93;
    f.r[1]=0x08b4e9bc;
    f.r[0]=0x14a1adfb;
    
    f.i[6]=0x00000000;
    f.i[5]=0x06d390fe;       
    f.i[4]=0xef6d88c8;
    f.i[3]=0xe3622220;
    f.i[2]=0xff239f8f;
    f.i[1]=0x1f890ece;
    f.i[0]=0x77257dde;
*/		

		time_f = clock();		
		dt1 = time_f - time_s;
		
	printf("TP_f_r: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
#if defined(SIXTEEN_BIT_PROCESSOR)
		printf("%x ", f.r[i]);
#elif defined(THIRTYTWO_BIT_PROCESSOR)
		printf("%x %x ", (uint16_t)(f.r[i] >> 16), (uint16_t) f.r[i]);
#endif
	}
	printf("\n");
	printf("TP_f_i: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
#if defined(SIXTEEN_BIT_PROCESSOR)
		printf("%x ", f.i[i]);
#elif defined(THIRTYTWO_BIT_PROCESSOR)
		printf("%x %x ", (uint16_t)(f.i[i] >> 16), (uint16_t) f.i[i]);
#endif
	}
	printf("\n");


		time_s = clock();

		TP_final_expon(&res, &f);

		time_f = clock();
		dt2 = time_f - time_s;
 
	printf("TP_res_r: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
#if defined(SIXTEEN_BIT_PROCESSOR)
		printf("%x ", res.r[i]);
#elif defined(THIRTYTWO_BIT_PROCESSOR)
		printf("%x %x ", (uint16_t)(res.r[i] >> 16), (uint16_t) res.r[i]);
#endif
	}
	printf("\n");
	printf("TP_res_i: ");
	for (i = NUMWORDS-1; i >= 0; i--) {
#if defined(SIXTEEN_BIT_PROCESSOR)
		printf("%x ", res.i[i]);
#elif defined(THIRTYTWO_BIT_PROCESSOR)
		printf("%x %x ", (uint16_t)(res.i[i] >> 16), (uint16_t) res.i[i]);
#endif
	}
	printf("\n");
 
      
      printf("TP= ");
//      print_val(res);
      printf("time(%d): init: %lu ms, miller: %lu ms, exp: %lu ms, miller+exp: %lu ms\n", 
			 round_index, (uint32_t)(dt0*1000/CLOCK_SECOND), 
					(uint32_t)(dt1*1000/CLOCK_SECOND), 
					(uint32_t)(dt2*1000/CLOCK_SECOND), 
					(uint32_t)((dt1+dt2)*1000/CLOCK_SECOND));

      round_index++;
      
    }
    
  }
  

/* scopes process */
int main(void)
{
	
	printf("TP tester process started\n");

	round_index = 0;

	printf("entering loop.\n");
	
		
	runTP();
		
}


