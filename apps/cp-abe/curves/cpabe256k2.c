/** 
 * Module for curve cpabe
 *
 * Author: Daniel Jacobi
 * Date: 02/02/2011
 */

#include "ECC.h"
#include "NN.h"
#include <string.h>

#ifdef CPABE256K2

void get_TP_param(TPParams *tppara)
  {
#ifdef EIGHT_BIT_PROCESSOR
	  //init parameters
	  //prime p
	  tppara->p[32] = 0x00;
	  tppara->p[31] = 0x22;
	  tppara->p[30] = 0xd4;
	  tppara->p[29] = 0xb7;
	  tppara->p[28] = 0xf1;
	  tppara->p[27] = 0xef;
	  tppara->p[26] = 0xf0;
	  tppara->p[25] = 0x26;
	  tppara->p[24] = 0x9e;
	  tppara->p[23] = 0x3b;
	  tppara->p[22] = 0xd4;
	  tppara->p[21] = 0x4c;
	  tppara->p[20] = 0x0d;
	  tppara->p[19] = 0x7c;
	  tppara->p[18] = 0xc9;
	  tppara->p[17] = 0x0a;
	  tppara->p[16] = 0x00;
	  tppara->p[15] = 0x00;
	  tppara->p[14] = 0x00;
	  tppara->p[13] = 0x00;
	  tppara->p[12] = 0x00;
	  tppara->p[11] = 0x45;
	  tppara->p[10] = 0xa9;
	  tppara->p[9] = 0x6f;
	  tppara->p[8] = 0x9e;
	  tppara->p[7] = 0x36;
	  tppara->p[6] = 0x70;
	  tppara->p[5] = 0xaf;
	  tppara->p[4] = 0x06;
	  tppara->p[3] = 0x06;
	  tppara->p[2] = 0xf9;
	  tppara->p[1] = 0x92;
	  tppara->p[0] = 0x13;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[32] = 0x00;
	  tppara->P.x[31] = 0x00;
	  tppara->P.x[30] = 0x00;
	  tppara->P.x[29] = 0x74;
	  tppara->P.x[28] = 0x87;
	  tppara->P.x[27] = 0x24;
	  tppara->P.x[26] = 0x4a;
	  tppara->P.x[25] = 0x5e;
	  tppara->P.x[24] = 0xb3;
	  tppara->P.x[23] = 0xb1;
	  tppara->P.x[22] = 0x51;
	  tppara->P.x[21] = 0xfd;
	  tppara->P.x[20] = 0x66;
	  tppara->P.x[19] = 0xe2;
	  tppara->P.x[18] = 0x42;
	  tppara->P.x[17] = 0x66;
	  tppara->P.x[16] = 0xb4;
	  tppara->P.x[15] = 0xf8;
	  tppara->P.x[14] = 0xa4;
	  tppara->P.x[13] = 0x17;
	  tppara->P.x[12] = 0xb1;
	  tppara->P.x[11] = 0xe5;
	  tppara->P.x[10] = 0xa3;
	  tppara->P.x[9] = 0x6e;
	  tppara->P.x[8] = 0x4a;
	  tppara->P.x[7] = 0x83;
	  tppara->P.x[6] = 0x8c;
	  tppara->P.x[5] = 0xb2;
	  tppara->P.x[4] = 0x84;
	  tppara->P.x[3] = 0x81;
	  tppara->P.x[2] = 0xb2;
	  tppara->P.x[1] = 0xe0;
	  tppara->P.x[0] = 0x9b;

	  tppara->P.y[32] = 0x00;
	  tppara->P.y[31] = 0x00;
	  tppara->P.y[30] = 0x00;
	  tppara->P.y[29] = 0x0d;
	  tppara->P.y[28] = 0xf0;
	  tppara->P.y[27] = 0xe6;
	  tppara->P.y[26] = 0x9c;
	  tppara->P.y[25] = 0xf9;
	  tppara->P.y[24] = 0x65;
	  tppara->P.y[23] = 0x02;
	  tppara->P.y[22] = 0x86;
	  tppara->P.y[21] = 0xac;
	  tppara->P.y[20] = 0x25;
	  tppara->P.y[19] = 0xd2;
	  tppara->P.y[18] = 0x0a;
	  tppara->P.y[17] = 0xb6;
	  tppara->P.y[16] = 0xae;
	  tppara->P.y[15] = 0x3a;
	  tppara->P.y[14] = 0xd2;
	  tppara->P.y[13] = 0x9d;
	  tppara->P.y[12] = 0xa2;
	  tppara->P.y[11] = 0x1a;
	  tppara->P.y[10] = 0x8e;
	  tppara->P.y[9] = 0xd5;
	  tppara->P.y[8] = 0x7d;
	  tppara->P.y[7] = 0x89;
	  tppara->P.y[6] = 0x86;
	  tppara->P.y[5] = 0x91;
	  tppara->P.y[4] = 0x25;
	  tppara->P.y[3] = 0xb8;
	  tppara->P.y[2] = 0xe7;
	  tppara->P.y[1] = 0xa5;
	  tppara->P.y[0] = 0x4b;

	  // group order m
	  tppara->m[32] = 0x00;
	  tppara->m[31] = 0x00;
	  tppara->m[30] = 0x00;
	  tppara->m[29] = 0x00;
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
	  tppara->c[32] = 0x00;
	  tppara->c[31] = 0x00;
	  tppara->c[30] = 0x00;
	  tppara->c[29] = 0x00;
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
	  tppara->c[11] = 0x45;
	  tppara->c[10] = 0xa9;
	  tppara->c[9] = 0x6f;
	  tppara->c[8] = 0x9e;
	  tppara->c[7] = 0x36;
	  tppara->c[6] = 0x70;
	  tppara->c[5] = 0xaf;
	  tppara->c[4] = 0x06;
	  tppara->c[3] = 0x06;
	  tppara->c[2] = 0xf9;
	  tppara->c[1] = 0x92;
	  tppara->c[0] = 0x14;
#elif defined(SIXTEEN_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[16] = 0x0000;
	  tppara->p[15] = 0x22d4;
	  tppara->p[14] = 0xb7f1;
	  tppara->p[13] = 0xeff0;
	  tppara->p[12] = 0x269e;
	  tppara->p[11] = 0x3bd4;
	  tppara->p[10] = 0x4c0d;
	  tppara->p[9] = 0x7cc9;
	  tppara->p[8] = 0x0a00;
	  tppara->p[7] = 0x0000;
	  tppara->p[6] = 0x0000;
	  tppara->p[5] = 0x45a9;
	  tppara->p[4] = 0x6f9e;
	  tppara->p[3] = 0x3670;
	  tppara->p[2] = 0xaf06;
	  tppara->p[1] = 0x06f9;
	  tppara->p[0] = 0x9213;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[16] = 0x0000;
	  tppara->P.x[15] = 0x0000;
	  tppara->P.x[14] = 0x7487;
	  tppara->P.x[13] = 0x244a;
	  tppara->P.x[12] = 0x5eb3;
	  tppara->P.x[11] = 0xb151;
	  tppara->P.x[10] = 0xfd66;
	  tppara->P.x[9] = 0xe242;
	  tppara->P.x[8] = 0x66b4;
	  tppara->P.x[7] = 0xf8a4;
	  tppara->P.x[6] = 0x17b1;
	  tppara->P.x[5] = 0xe5a3;
	  tppara->P.x[4] = 0x6e4a;
	  tppara->P.x[3] = 0x838c;
	  tppara->P.x[2] = 0xb284;
	  tppara->P.x[1] = 0x81b2;
	  tppara->P.x[0] = 0xe09b;

	  tppara->P.y[16] = 0x0000;
	  tppara->P.y[15] = 0x0000;
	  tppara->P.y[14] = 0x0df0;
	  tppara->P.y[13] = 0xe69c;
	  tppara->P.y[12] = 0xf965;
	  tppara->P.y[11] = 0x0286;
	  tppara->P.y[10] = 0xac25;
	  tppara->P.y[9] = 0xd20a;
	  tppara->P.y[8] = 0xb6ae;
	  tppara->P.y[7] = 0x3ad2;
	  tppara->P.y[6] = 0x9da2;
	  tppara->P.y[5] = 0x1a8e;
	  tppara->P.y[4] = 0xd57d;
	  tppara->P.y[3] = 0x8986;
	  tppara->P.y[2] = 0x9125;
	  tppara->P.y[1] = 0xb8e7;
	  tppara->P.y[0] = 0xa54b;

	  // group order m
	  tppara->m[16] = 0x0000;
	  tppara->m[15] = 0x0000;
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
	  tppara->c[16] = 0x0000;
	  tppara->c[15] = 0x0000;
	  tppara->c[14] = 0x0000;
	  tppara->c[13] = 0x0000;
	  tppara->c[12] = 0x0000;
	  tppara->c[11] = 0x0000;
	  tppara->c[10] = 0x0000;
	  tppara->c[9] = 0x0000;
	  tppara->c[8] = 0x0000;
	  tppara->c[7] = 0x0000;
	  tppara->c[6] = 0x0000;
	  tppara->c[5] = 0x45a9;
	  tppara->c[4] = 0x6f9e;
	  tppara->c[3] = 0x3670;
	  tppara->c[2] = 0xaf06;
	  tppara->c[1] = 0x06f9;
	  tppara->c[0] = 0x9214;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[8] = 0x00000000;
	  tppara->p[7] = 0x22d4b7f1;
	  tppara->p[6] = 0xeff0269e;
	  tppara->p[5] = 0x3bd44c0d;
	  tppara->p[4] = 0x7cc90a00;
	  tppara->p[3] = 0x00000000;
	  tppara->p[2] = 0x45a96f9e;
	  tppara->p[1] = 0x3670af06;
	  tppara->p[0] = 0x06f99213;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[8] = 0x00000000;
	  tppara->P.x[7] = 0x00007487;
	  tppara->P.x[6] = 0x244a5eb3;
	  tppara->P.x[5] = 0xb151fd66;
	  tppara->P.x[4] = 0xe24266b4;
	  tppara->P.x[3] = 0xf8a417b1;
	  tppara->P.x[2] = 0xe5a36e4a;
	  tppara->P.x[1] = 0x838cb284;
	  tppara->P.x[0] = 0x81b2e09b;

	  tppara->P.y[8] = 0x00000000;
	  tppara->P.y[7] = 0x00000df0;
	  tppara->P.y[6] = 0xe69cf965;
	  tppara->P.y[5] = 0x0286ac25;
	  tppara->P.y[4] = 0xd20ab6ae;
	  tppara->P.y[3] = 0x3ad29da2;
	  tppara->P.y[2] = 0x1a8ed57d;
	  tppara->P.y[1] = 0x89869125;
	  tppara->P.y[0] = 0xb8e7a54b;

	  // group order m
	  tppara->m[8] = 0x00000000;
	  tppara->m[7] = 0x00000000;
	  tppara->m[6] = 0x00000000;
	  tppara->m[5] = 0x00000000;
	  tppara->m[4] = 0x80000080;
	  tppara->m[3] = 0x00000000;
	  tppara->m[2] = 0x00000000;
	  tppara->m[1] = 0x00000000;
	  tppara->m[0] = 0x00000001;

	// c= ((p^(k-1))+1)/m
	  tppara->c[8] = 0x00000000;
	  tppara->c[7] = 0x00000000;
	  tppara->c[6] = 0x00000000;
	  tppara->c[5] = 0x00000000;
	  tppara->c[4] = 0x00000000;
	  tppara->c[3] = 0x00000000;
	  tppara->c[2] = 0x45a96f9e;
	  tppara->c[1] = 0x3670af06;
	  tppara->c[0] = 0x06f99214;
	  
 #endif
  }

#endif
