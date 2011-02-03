/** 

/**
 * Module for curve cpabe
 *
 * Author: Daniel Jacobi
 * Date: 02/02/2011
 */

#include "ECC.h"
#include "NN.h"
#include <string.h>

void get_TP_param(TPParams *tppara)
  {
#ifdef EIGHT_BIT_PROCESSOR
	  //init parameters
	  //prime p
	  tppara->p[28] = 0x00;
	  tppara->p[27] = 0xa7;
	  tppara->p[26] = 0xe3;
	  tppara->p[25] = 0x75;
	  tppara->p[24] = 0x2a;
	  tppara->p[23] = 0x31;
	  tppara->p[22] = 0x4e;
	  tppara->p[21] = 0xbb;
	  tppara->p[20] = 0x57;
	  tppara->p[19] = 0xda;
	  tppara->p[18] = 0x39;
	  tppara->p[17] = 0x0a;
	  tppara->p[16] = 0x00;
	  tppara->p[15] = 0x00;
	  tppara->p[14] = 0x00;
	  tppara->p[13] = 0x00;
	  tppara->p[12] = 0x00;
	  tppara->p[11] = 0x00;
	  tppara->p[10] = 0x00;
	  tppara->p[9] = 0x00;
	  tppara->p[8] = 0x01;
	  tppara->p[7] = 0x4f;
	  tppara->p[6] = 0xc6;
	  tppara->p[5] = 0xe9;
	  tppara->p[4] = 0x04;
	  tppara->p[3] = 0x9b;
	  tppara->p[2] = 0xb4;
	  tppara->p[1] = 0x72;
	  tppara->p[0] = 0x13;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x01;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[28] = 0x00;
	  tppara->P.x[27] = 0x4d;
	  tppara->P.x[26] = 0x64;
	  tppara->P.x[25] = 0x3d;
	  tppara->P.x[24] = 0x6b;
	  tppara->P.x[23] = 0x42;
	  tppara->P.x[22] = 0x59;
	  tppara->P.x[21] = 0xb3;
	  tppara->P.x[20] = 0x8b;
	  tppara->P.x[19] = 0x9a;
	  tppara->P.x[18] = 0x5e;
	  tppara->P.x[17] = 0x95;
	  tppara->P.x[16] = 0x27;
	  tppara->P.x[15] = 0x46;
	  tppara->P.x[14] = 0xae;
	  tppara->P.x[13] = 0x31;
	  tppara->P.x[12] = 0x4e;
	  tppara->P.x[11] = 0x37;
	  tppara->P.x[10] = 0x24;
	  tppara->P.x[9] = 0xfc;
	  tppara->P.x[8] = 0x17;
	  tppara->P.x[7] = 0x8a;
	  tppara->P.x[6] = 0x7f;
	  tppara->P.x[5] = 0x84;
	  tppara->P.x[4] = 0x32;
	  tppara->P.x[3] = 0xcb;
	  tppara->P.x[2] = 0x79;
	  tppara->P.x[1] = 0x34;
	  tppara->P.x[0] = 0x6a;

	  tppara->P.y[28] = 0x00;
	  tppara->P.y[27] = 0x00;
	  tppara->P.y[26] = 0x02;
	  tppara->P.y[25] = 0x89;
	  tppara->P.y[24] = 0x7d;
	  tppara->P.y[23] = 0x4c;
	  tppara->P.y[22] = 0x1a;
	  tppara->P.y[21] = 0xa8;
	  tppara->P.y[20] = 0x94;
	  tppara->P.y[19] = 0x98;
	  tppara->P.y[18] = 0x68;
	  tppara->P.y[17] = 0x5a;
	  tppara->P.y[16] = 0x97;
	  tppara->P.y[15] = 0x2b;
	  tppara->P.y[14] = 0xb7;
	  tppara->P.y[13] = 0x92;
	  tppara->P.y[12] = 0x99;
	  tppara->P.y[11] = 0xae;
	  tppara->P.y[10] = 0x97;
	  tppara->P.y[9] = 0x2e;
	  tppara->P.y[8] = 0x36;
	  tppara->P.y[7] = 0x2a;
	  tppara->P.y[6] = 0xc2;
	  tppara->P.y[5] = 0xbf;
	  tppara->P.y[4] = 0x9c;
	  tppara->P.y[3] = 0xcd;
	  tppara->P.y[2] = 0x3b;
	  tppara->P.y[1] = 0xa3;
	  tppara->P.y[0] = 0x77;

	  // group order m
	  tppara->m[28] = 0x00;
	  tppara->m[27] = 0x00;
	  tppara->m[26] = 0x00;
	  tppara->m[25] = 0x00;
	  tppara->m[24] = 0x00;
	  tppara->m[23] = 0x00;
	  tppara->m[22] = 0x00;
	  tppara->m[21] = 0x00;
	  tppara->m[20] = 0x00;
	  tppara->m[19] = 0x80;
	  tppara->m[18] = 0x00;
	  tppara->m[17] = 0x00;
	  tppara->m[16] = 0x80;
	  tppara->m[15] = 0x00;
	  tppara->m[14] = 0x00;
	  tppara->m[13] = 0x00;
	  tppara->m[12] = 0x00;
	  tppara->m[11] = 0x00;
	  tppara->m[10] = 0x00;
	  tppara->m[9] = 0x00;
	  tppara->m[8] = 0x00;
	  tppara->m[7] = 0x00;
	  tppara->m[6] = 0x00;
	  tppara->m[5] = 0x00;
	  tppara->m[4] = 0x00;
	  tppara->m[3] = 0x00;
	  tppara->m[2] = 0x00;
	  tppara->m[1] = 0x00;
	  tppara->m[0] = 0x01;

	// c= ((p^(k-1))+1)/m
	  tppara->c[28] = 0x00;
	  tppara->c[27] = 0x00;
	  tppara->c[26] = 0x00;
	  tppara->c[25] = 0x00;
	  tppara->c[24] = 0x00;
	  tppara->c[23] = 0x00;
	  tppara->c[22] = 0x00;
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
	  tppara->c[8] = 0x01;
	  tppara->c[7] = 0x4f;
	  tppara->c[6] = 0xc6;
	  tppara->c[5] = 0xe9;
	  tppara->c[4] = 0x04;
	  tppara->c[3] = 0x9b;
	  tppara->c[2] = 0xb4;
	  tppara->c[1] = 0x72;
	  tppara->c[0] = 0x14;
#elif defined(SIXTEEN_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[14] = 0x0000;
	  tppara->p[13] = 0xa7e3;
	  tppara->p[12] = 0x752a;
	  tppara->p[11] = 0x314e;
	  tppara->p[10] = 0xbb57;
	  tppara->p[9] = 0xda39;
	  tppara->p[8] = 0x0a00;
	  tppara->p[7] = 0x0000;
	  tppara->p[6] = 0x0000;
	  tppara->p[5] = 0x0000;
	  tppara->p[4] = 0x0001;
	  tppara->p[3] = 0x4fc6;
	  tppara->p[2] = 0xe904;
	  tppara->p[1] = 0x9bb4;
	  tppara->p[0] = 0x7213;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[14] = 0x0000;
	  tppara->P.x[13] = 0x4d64;
	  tppara->P.x[12] = 0x3d6b;
	  tppara->P.x[11] = 0x4259;
	  tppara->P.x[10] = 0xb38b;
	  tppara->P.x[9] = 0x9a5e;
	  tppara->P.x[8] = 0x9527;
	  tppara->P.x[7] = 0x46ae;
	  tppara->P.x[6] = 0x314e;
	  tppara->P.x[5] = 0x3724;
	  tppara->P.x[4] = 0xfc17;
	  tppara->P.x[3] = 0x8a7f;
	  tppara->P.x[2] = 0x8432;
	  tppara->P.x[1] = 0xcb79;
	  tppara->P.x[0] = 0x346a;

	  tppara->P.y[14] = 0x0000;
	  tppara->P.y[13] = 0x0002;
	  tppara->P.y[12] = 0x897d;
	  tppara->P.y[11] = 0x4c1a;
	  tppara->P.y[10] = 0xa894;
	  tppara->P.y[9] = 0x9868;
	  tppara->P.y[8] = 0x5a97;
	  tppara->P.y[7] = 0x2bb7;
	  tppara->P.y[6] = 0x9299;
	  tppara->P.y[5] = 0xae97;
	  tppara->P.y[4] = 0x2e36;
	  tppara->P.y[3] = 0x2ac2;
	  tppara->P.y[2] = 0xbf9c;
	  tppara->P.y[1] = 0xcd3b;
	  tppara->P.y[0] = 0xa377;

	  // group order m
	  tppara->m[14] = 0x0000;
	  tppara->m[13] = 0x0000;
	  tppara->m[12] = 0x0000;
	  tppara->m[11] = 0x0000;
	  tppara->m[10] = 0x0000;
	  tppara->m[9] = 0x8000;
	  tppara->m[8] = 0x0080;
	  tppara->m[7] = 0x0000;
	  tppara->m[6] = 0x0000;
	  tppara->m[5] = 0x0000;
	  tppara->m[4] = 0x0000;
	  tppara->m[3] = 0x0000;
	  tppara->m[2] = 0x0000;
	  tppara->m[1] = 0x0000;
	  tppara->m[0] = 0x0001;

	// c= ((p^(k-1))+1)/m
	  tppara->c[14] = 0x0000;
	  tppara->c[13] = 0x0000;
	  tppara->c[12] = 0x0000;
	  tppara->c[11] = 0x0000;
	  tppara->c[10] = 0x0000;
	  tppara->c[9] = 0x0000;
	  tppara->c[8] = 0x0000;
	  tppara->c[7] = 0x0000;
	  tppara->c[6] = 0x0000;
	  tppara->c[5] = 0x0000;
	  tppara->c[4] = 0x0001;
	  tppara->c[3] = 0x4fc6;
	  tppara->c[2] = 0xe904;
	  tppara->c[1] = 0x9bb4;
	  tppara->c[0] = 0x7214;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[7] = 0x00000000;
	  tppara->p[6] = 0xa7e3752a;
	  tppara->p[5] = 0x314ebb57;
	  tppara->p[4] = 0xda390a00;
	  tppara->p[3] = 0x00000000;
	  tppara->p[2] = 0x00000001;
	  tppara->p[1] = 0x4fc6e904;
	  tppara->p[0] = 0x9bb47213;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x00000001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[7] = 0x00000000;
	  tppara->P.x[6] = 0x4d643d6b;
	  tppara->P.x[5] = 0x4259b38b;
	  tppara->P.x[4] = 0x9a5e9527;
	  tppara->P.x[3] = 0x46ae314e;
	  tppara->P.x[2] = 0x3724fc17;
	  tppara->P.x[1] = 0x8a7f8432;
	  tppara->P.x[0] = 0xcb79346a;

	  tppara->P.y[7] = 0x00000000;
	  tppara->P.y[6] = 0x0002897d;
	  tppara->P.y[5] = 0x4c1aa894;
	  tppara->P.y[4] = 0x98685a97;
	  tppara->P.y[3] = 0x2bb79299;
	  tppara->P.y[2] = 0xae972e36;
	  tppara->P.y[1] = 0x2ac2bf9c;
	  tppara->P.y[0] = 0xcd3ba377;

	  // group order m
	  tppara->m[7] = 0x00000000;
	  tppara->m[6] = 0x00000000;
	  tppara->m[5] = 0x00000000;
	  tppara->m[4] = 0x80000080;
	  tppara->m[3] = 0x00000000;
	  tppara->m[2] = 0x00000000;
	  tppara->m[1] = 0x00000000;
	  tppara->m[0] = 0x00000001;

	// c= ((p^(k-1))+1)/m
	  tppara->c[7] = 0x00000000;
	  tppara->c[6] = 0x00000000;
	  tppara->c[5] = 0x00000000;
	  tppara->c[4] = 0x00000000;
	  tppara->c[3] = 0x00000000;
	  tppara->c[2] = 0x00000001;
	  tppara->c[1] = 0x4fc6e904;
	  tppara->c[0] = 0x9bb47214;
	  
 #endif
  }
