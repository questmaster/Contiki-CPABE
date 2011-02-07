/**
 * Module for curve cpabe
 *
 * Author: Daniel Jacobi
 * Date: 02/02/2011
 */

#include "ECC.h"
#include "NN.h"
#include <string.h>

#ifdef CPABE168K2

void get_TP_param(TPParams *tppara)
  {
#ifdef EIGHT_BIT_PROCESSOR
	  //init parameters
	  //prime p
	  tppara->p[21] = 0x00;
	  tppara->p[20] = 0x24;
	  tppara->p[19] = 0x00;
	  tppara->p[18] = 0x01;
	  tppara->p[17] = 0x1f;
	  tppara->p[16] = 0xff;
	  tppara->p[15] = 0xff;
	  tppara->p[14] = 0xff;
	  tppara->p[13] = 0xff;
	  tppara->p[12] = 0xff;
	  tppara->p[11] = 0xff;
	  tppara->p[10] = 0xff;
	  tppara->p[9] = 0xff;
	  tppara->p[8] = 0xff;
	  tppara->p[7] = 0xff;
	  tppara->p[6] = 0xff;
	  tppara->p[5] = 0xff;
	  tppara->p[4] = 0xff;
	  tppara->p[3] = 0xff;
	  tppara->p[2] = 0xff;
	  tppara->p[1] = 0xff;
	  tppara->p[0] = 0xb7;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[21] = 0x00;
	  tppara->P.x[20] = 0x00;
	  tppara->P.x[19] = 0x75;
	  tppara->P.x[18] = 0x89;
	  tppara->P.x[17] = 0x44;
	  tppara->P.x[16] = 0xf3;
	  tppara->P.x[15] = 0x7a;
	  tppara->P.x[14] = 0x81;
	  tppara->P.x[13] = 0x3f;
	  tppara->P.x[12] = 0x3c;
	  tppara->P.x[11] = 0x36;
	  tppara->P.x[10] = 0x74;
	  tppara->P.x[9] = 0xd3;
	  tppara->P.x[8] = 0x77;
	  tppara->P.x[7] = 0xb6;
	  tppara->P.x[6] = 0x8e;
	  tppara->P.x[5] = 0x3d;
	  tppara->P.x[4] = 0x19;
	  tppara->P.x[3] = 0x50;
	  tppara->P.x[2] = 0xb6;
	  tppara->P.x[1] = 0x53;
	  tppara->P.x[0] = 0xd3;

	  tppara->P.y[21] = 0x00;
	  tppara->P.y[20] = 0x12;
	  tppara->P.y[19] = 0x5f;
	  tppara->P.y[18] = 0xad;
	  tppara->P.y[17] = 0xfe;
	  tppara->P.y[16] = 0x8b;
	  tppara->P.y[15] = 0x82;
	  tppara->P.y[14] = 0x59;
	  tppara->P.y[13] = 0x45;
	  tppara->P.y[12] = 0xc6;
	  tppara->P.y[11] = 0x8b;
	  tppara->P.y[10] = 0x74;
	  tppara->P.y[9] = 0x6a;
	  tppara->P.y[8] = 0x80;
	  tppara->P.y[7] = 0x30;
	  tppara->P.y[6] = 0x23;
	  tppara->P.y[5] = 0x42;
	  tppara->P.y[4] = 0xd8;
	  tppara->P.y[3] = 0x54;
	  tppara->P.y[2] = 0x30;
	  tppara->P.y[1] = 0xac;
	  tppara->P.y[0] = 0xd1;

	  // group order m
	  tppara->m[21] = 0x00;
	  tppara->m[20] = 0x00;
	  tppara->m[19] = 0x80;
	  tppara->m[18] = 0x00;
	  tppara->m[17] = 0x03;
	  tppara->m[16] = 0xff;
	  tppara->m[15] = 0xff;
	  tppara->m[14] = 0xff;
	  tppara->m[13] = 0xff;
	  tppara->m[12] = 0xff;
	  tppara->m[11] = 0xff;
	  tppara->m[10] = 0xff;
	  tppara->m[9] = 0xff;
	  tppara->m[8] = 0xff;
	  tppara->m[7] = 0xff;
	  tppara->m[6] = 0xff;
	  tppara->m[5] = 0xff;
	  tppara->m[4] = 0xff;
	  tppara->m[3] = 0xff;
	  tppara->m[2] = 0xff;
	  tppara->m[1] = 0xff;
	  tppara->m[0] = 0xff;

	// c= ((p^(k-1))+1)/m
	  tppara->c[21] = 0x00;
	  tppara->c[20] = 0x00;
	  tppara->c[19] = 0x00;
	  tppara->c[18] = 0x00;
	  tppara->c[17] = 0x00;
	  tppara->c[16] = 0x00;
	  tppara->c[15] = 0x00;
	  tppara->c[14] = 0x00;
	  tppara->c[13] = 0x00;
	  tppara->c[12] = 0x00;
	  tppara->c[11] = 0x00;
	  tppara->c[10] = 0x00;
	  tppara->c[9] = 0x00;
	  tppara->c[8] = 0x00;
	  tppara->c[7] = 0x00;
	  tppara->c[6] = 0x00;
	  tppara->c[5] = 0x00;
	  tppara->c[4] = 0x00;
	  tppara->c[3] = 0x00;
	  tppara->c[2] = 0x00;
	  tppara->c[1] = 0x00;
	  tppara->c[0] = 0x48;
#elif defined(SIXTEEN_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[11] = 0x0000;
	  tppara->p[10] = 0x0024;
	  tppara->p[9] = 0x0001;
	  tppara->p[8] = 0x1fff;
	  tppara->p[7] = 0xffff;
	  tppara->p[6] = 0xffff;
	  tppara->p[5] = 0xffff;
	  tppara->p[4] = 0xffff;
	  tppara->p[3] = 0xffff;
	  tppara->p[2] = 0xffff;
	  tppara->p[1] = 0xffff;
	  tppara->p[0] = 0xffb7;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[11] = 0x0000;
	  tppara->P.x[10] = 0x0000;
	  tppara->P.x[9] = 0x7589;
	  tppara->P.x[8] = 0x44f3;
	  tppara->P.x[7] = 0x7a81;
	  tppara->P.x[6] = 0x3f3c;
	  tppara->P.x[5] = 0x3674;
	  tppara->P.x[4] = 0xd377;
	  tppara->P.x[3] = 0xb68e;
	  tppara->P.x[2] = 0x3d19;
	  tppara->P.x[1] = 0x50b6;
	  tppara->P.x[0] = 0x53d3;

	  tppara->P.y[11] = 0x0000;
	  tppara->P.y[10] = 0x0012;
	  tppara->P.y[9] = 0x5fad;
	  tppara->P.y[8] = 0xfe8b;
	  tppara->P.y[7] = 0x8259;
	  tppara->P.y[6] = 0x45c6;
	  tppara->P.y[5] = 0x8b74;
	  tppara->P.y[4] = 0x6a80;
	  tppara->P.y[3] = 0x3023;
	  tppara->P.y[2] = 0x42d8;
	  tppara->P.y[1] = 0x5430;
	  tppara->P.y[0] = 0xacd1;

	  // group order m
	  tppara->m[11] = 0x0000;
	  tppara->m[10] = 0x0000;
	  tppara->m[9] = 0x8000;
	  tppara->m[8] = 0x03ff;
	  tppara->m[7] = 0xffff;
	  tppara->m[6] = 0xffff;
	  tppara->m[5] = 0xffff;
	  tppara->m[4] = 0xffff;
	  tppara->m[3] = 0xffff;
	  tppara->m[2] = 0xffff;
	  tppara->m[1] = 0xffff;
	  tppara->m[0] = 0xffff;

	// c= ((p^(k-1))+1)/m
	  tppara->c[11] = 0x0000;
	  tppara->c[10] = 0x0000;
	  tppara->c[9] = 0x0000;
	  tppara->c[8] = 0x0000;
	  tppara->c[7] = 0x0000;
	  tppara->c[6] = 0x0000;
	  tppara->c[5] = 0x0000;
	  tppara->c[4] = 0x0000;
	  tppara->c[3] = 0x0000;
	  tppara->c[2] = 0x0000;
	  tppara->c[1] = 0x0000;
	  tppara->c[0] = 0x0048;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[6] = 0x00000000;
	  tppara->p[5] = 0x00000024;
	  tppara->p[4] = 0x00011fff;
	  tppara->p[3] = 0xffffffff;
	  tppara->p[2] = 0xffffffff;
	  tppara->p[1] = 0xffffffff;
	  tppara->p[0] = 0xffffffb7;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[6] = 0x00000000;
	  tppara->P.x[5] = 0x00000000;
	  tppara->P.x[4] = 0x758944f3;
	  tppara->P.x[3] = 0x7a813f3c;
	  tppara->P.x[2] = 0x3674d377;
	  tppara->P.x[1] = 0xb68e3d19;
	  tppara->P.x[0] = 0x50b653d3;

	  tppara->P.y[6] = 0x00000000;
	  tppara->P.y[5] = 0x00000012;
	  tppara->P.y[4] = 0x5fadfe8b;
	  tppara->P.y[3] = 0x825945c6;
	  tppara->P.y[2] = 0x8b746a80;
	  tppara->P.y[1] = 0x302342d8;
	  tppara->P.y[0] = 0x5430acd1;

	  // group order m
	  tppara->m[6] = 0x00000000;
	  tppara->m[5] = 0x00000000;
	  tppara->m[4] = 0x800003ff;
	  tppara->m[3] = 0xffffffff;
	  tppara->m[2] = 0xffffffff;
	  tppara->m[1] = 0xffffffff;
	  tppara->m[0] = 0xffffffff;

	// c= ((p^(k-1))+1)/m
	  tppara->c[6] = 0x00000000;
	  tppara->c[5] = 0x00000000;
	  tppara->c[4] = 0x00000000;
	  tppara->c[3] = 0x00000000;
	  tppara->c[2] = 0x00000000;
	  tppara->c[1] = 0x00000000;
	  tppara->c[0] = 0x00000048;
	  
 #endif
  }

#endif
