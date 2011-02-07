/** 
 * Module for curve cpabe
 *
 * Author: Daniel Jacobi
 * Date: 02/02/2011
 */

#include "ECC.h"
#include "NN.h"
#include <string.h>

#ifdef CPABE384K2

void get_TP_param(TPParams *tppara)
  {
#ifdef EIGHT_BIT_PROCESSOR
	  //init parameters
	  //prime p
	  tppara->p[48] = 0x00;
	  tppara->p[47] = 0x9f;
	  tppara->p[46] = 0xe1;
	  tppara->p[45] = 0x00;
	  tppara->p[44] = 0x43;
	  tppara->p[43] = 0xe7;
	  tppara->p[42] = 0x69;
	  tppara->p[41] = 0xf5;
	  tppara->p[40] = 0xe5;
	  tppara->p[39] = 0xf9;
	  tppara->p[38] = 0xc7;
	  tppara->p[37] = 0x6e;
	  tppara->p[36] = 0x5b;
	  tppara->p[35] = 0xb3;
	  tppara->p[34] = 0x52;
	  tppara->p[33] = 0x5c;
	  tppara->p[32] = 0x50;
	  tppara->p[31] = 0x4e;
	  tppara->p[30] = 0x37;
	  tppara->p[29] = 0x61;
	  tppara->p[28] = 0xa2;
	  tppara->p[27] = 0x91;
	  tppara->p[26] = 0xe0;
	  tppara->p[25] = 0x40;
	  tppara->p[24] = 0xdb;
	  tppara->p[23] = 0xe3;
	  tppara->p[22] = 0x60;
	  tppara->p[21] = 0x00;
	  tppara->p[20] = 0x54;
	  tppara->p[19] = 0xb9;
	  tppara->p[18] = 0x40;
	  tppara->p[17] = 0x5a;
	  tppara->p[16] = 0xfd;
	  tppara->p[15] = 0x9b;
	  tppara->p[14] = 0x83;
	  tppara->p[13] = 0x5a;
	  tppara->p[12] = 0x83;
	  tppara->p[11] = 0x48;
	  tppara->p[10] = 0xbd;
	  tppara->p[9] = 0x22;
	  tppara->p[8] = 0x72;
	  tppara->p[7] = 0x73;
	  tppara->p[6] = 0xa7;
	  tppara->p[5] = 0xfd;
	  tppara->p[4] = 0x97;
	  tppara->p[3] = 0x7b;
	  tppara->p[2] = 0x14;
	  tppara->p[1] = 0x07;
	  tppara->p[0] = 0xd7;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[48] = 0x00;
	  tppara->P.x[47] = 0x08;
	  tppara->P.x[46] = 0x7d;
	  tppara->P.x[45] = 0x45;
	  tppara->P.x[44] = 0x69;
	  tppara->P.x[43] = 0xe4;
	  tppara->P.x[42] = 0xea;
	  tppara->P.x[41] = 0x57;
	  tppara->P.x[40] = 0x19;
	  tppara->P.x[39] = 0xe8;
	  tppara->P.x[38] = 0x4f;
	  tppara->P.x[37] = 0x8c;
	  tppara->P.x[36] = 0x0f;
	  tppara->P.x[35] = 0xf6;
	  tppara->P.x[34] = 0xfe;
	  tppara->P.x[33] = 0xd8;
	  tppara->P.x[32] = 0xa7;
	  tppara->P.x[31] = 0x18;
	  tppara->P.x[30] = 0x75;
	  tppara->P.x[29] = 0xa8;
	  tppara->P.x[28] = 0x92;
	  tppara->P.x[27] = 0x77;
	  tppara->P.x[26] = 0xc9;
	  tppara->P.x[25] = 0xb1;
	  tppara->P.x[24] = 0xd1;
	  tppara->P.x[23] = 0x68;
	  tppara->P.x[22] = 0x56;
	  tppara->P.x[21] = 0xaf;
	  tppara->P.x[20] = 0x9d;
	  tppara->P.x[19] = 0xbf;
	  tppara->P.x[18] = 0x95;
	  tppara->P.x[17] = 0x2f;
	  tppara->P.x[16] = 0x30;
	  tppara->P.x[15] = 0x30;
	  tppara->P.x[14] = 0x60;
	  tppara->P.x[13] = 0xe6;
	  tppara->P.x[12] = 0xe4;
	  tppara->P.x[11] = 0x6f;
	  tppara->P.x[10] = 0xcb;
	  tppara->P.x[9] = 0xc0;
	  tppara->P.x[8] = 0xc7;
	  tppara->P.x[7] = 0x5b;
	  tppara->P.x[6] = 0x72;
	  tppara->P.x[5] = 0xb9;
	  tppara->P.x[4] = 0xad;
	  tppara->P.x[3] = 0x10;
	  tppara->P.x[2] = 0x8b;
	  tppara->P.x[1] = 0x38;
	  tppara->P.x[0] = 0xdb;

	  tppara->P.y[48] = 0x00;
	  tppara->P.y[47] = 0x00;
	  tppara->P.y[46] = 0x01;
	  tppara->P.y[45] = 0xa8;
	  tppara->P.y[44] = 0x95;
	  tppara->P.y[43] = 0xb2;
	  tppara->P.y[42] = 0xde;
	  tppara->P.y[41] = 0x86;
	  tppara->P.y[40] = 0x83;
	  tppara->P.y[39] = 0xb1;
	  tppara->P.y[38] = 0x05;
	  tppara->P.y[37] = 0x71;
	  tppara->P.y[36] = 0x76;
	  tppara->P.y[35] = 0xb3;
	  tppara->P.y[34] = 0x8c;
	  tppara->P.y[33] = 0xee;
	  tppara->P.y[32] = 0x52;
	  tppara->P.y[31] = 0x2e;
	  tppara->P.y[30] = 0x62;
	  tppara->P.y[29] = 0xab;
	  tppara->P.y[28] = 0xb3;
	  tppara->P.y[27] = 0xff;
	  tppara->P.y[26] = 0x33;
	  tppara->P.y[25] = 0xc3;
	  tppara->P.y[24] = 0x45;
	  tppara->P.y[23] = 0xbe;
	  tppara->P.y[22] = 0xf6;
	  tppara->P.y[21] = 0xe0;
	  tppara->P.y[20] = 0x44;
	  tppara->P.y[19] = 0x14;
	  tppara->P.y[18] = 0x77;
	  tppara->P.y[17] = 0xa0;
	  tppara->P.y[16] = 0x7b;
	  tppara->P.y[15] = 0xc8;
	  tppara->P.y[14] = 0xfb;
	  tppara->P.y[13] = 0x3d;
	  tppara->P.y[12] = 0x34;
	  tppara->P.y[11] = 0x97;
	  tppara->P.y[10] = 0x58;
	  tppara->P.y[9] = 0x11;
	  tppara->P.y[8] = 0xe0;
	  tppara->P.y[7] = 0x61;
	  tppara->P.y[6] = 0xa5;
	  tppara->P.y[5] = 0xae;
	  tppara->P.y[4] = 0xd5;
	  tppara->P.y[3] = 0x7c;
	  tppara->P.y[2] = 0x99;
	  tppara->P.y[1] = 0xb6;
	  tppara->P.y[0] = 0x48;

	  // group order m
	  tppara->m[48] = 0x00;
	  tppara->m[47] = 0x00;
	  tppara->m[46] = 0x00;
	  tppara->m[45] = 0x00;
	  tppara->m[44] = 0x00;
	  tppara->m[43] = 0x00;
	  tppara->m[42] = 0x00;
	  tppara->m[41] = 0x00;
	  tppara->m[40] = 0x00;
	  tppara->m[39] = 0x00;
	  tppara->m[38] = 0x00;
	  tppara->m[37] = 0x00;
	  tppara->m[36] = 0x00;
	  tppara->m[35] = 0x00;
	  tppara->m[34] = 0x00;
	  tppara->m[33] = 0x00;
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
	  tppara->c[48] = 0x00;
	  tppara->c[47] = 0x00;
	  tppara->c[46] = 0x00;
	  tppara->c[45] = 0x00;
	  tppara->c[44] = 0x00;
	  tppara->c[43] = 0x00;
	  tppara->c[42] = 0x00;
	  tppara->c[41] = 0x00;
	  tppara->c[40] = 0x00;
	  tppara->c[39] = 0x00;
	  tppara->c[38] = 0x00;
	  tppara->c[37] = 0x00;
	  tppara->c[36] = 0x00;
	  tppara->c[35] = 0x00;
	  tppara->c[34] = 0x00;
	  tppara->c[33] = 0x00;
	  tppara->c[32] = 0x00;
	  tppara->c[31] = 0x00;
	  tppara->c[30] = 0x00;
	  tppara->c[29] = 0x00;
	  tppara->c[28] = 0x01;
	  tppara->c[27] = 0x3f;
	  tppara->c[26] = 0xc1;
	  tppara->c[25] = 0xf6;
	  tppara->c[24] = 0x89;
	  tppara->c[23] = 0xbf;
	  tppara->c[22] = 0x1f;
	  tppara->c[21] = 0x9d;
	  tppara->c[20] = 0xd2;
	  tppara->c[19] = 0xf6;
	  tppara->c[18] = 0xa0;
	  tppara->c[17] = 0x45;
	  tppara->c[16] = 0x02;
	  tppara->c[15] = 0x64;
	  tppara->c[14] = 0x7c;
	  tppara->c[13] = 0xa5;
	  tppara->c[12] = 0x7c;
	  tppara->c[11] = 0xb7;
	  tppara->c[10] = 0x42;
	  tppara->c[9] = 0xdd;
	  tppara->c[8] = 0x8d;
	  tppara->c[7] = 0x8c;
	  tppara->c[6] = 0x58;
	  tppara->c[5] = 0x02;
	  tppara->c[4] = 0x68;
	  tppara->c[3] = 0x84;
	  tppara->c[2] = 0xeb;
	  tppara->c[1] = 0xf8;
	  tppara->c[0] = 0x28;
#elif defined(SIXTEEN_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[24] = 0x0000;
	  tppara->p[23] = 0x9fe1;
	  tppara->p[22] = 0x0043;
	  tppara->p[21] = 0xe769;
	  tppara->p[20] = 0xf5e5;
	  tppara->p[19] = 0xf9c7;
	  tppara->p[18] = 0x6e5b;
	  tppara->p[17] = 0xb352;
	  tppara->p[16] = 0x5c50;
	  tppara->p[15] = 0x4e37;
	  tppara->p[14] = 0x61a2;
	  tppara->p[13] = 0x91e0;
	  tppara->p[12] = 0x40db;
	  tppara->p[11] = 0xe360;
	  tppara->p[10] = 0x0054;
	  tppara->p[9] = 0xb940;
	  tppara->p[8] = 0x5afd;
	  tppara->p[7] = 0x9b83;
	  tppara->p[6] = 0x5a83;
	  tppara->p[5] = 0x48bd;
	  tppara->p[4] = 0x2272;
	  tppara->p[3] = 0x73a7;
	  tppara->p[2] = 0xfd97;
	  tppara->p[1] = 0x7b14;
	  tppara->p[0] = 0x07d7;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[24] = 0x0000;
	  tppara->P.x[23] = 0x087d;
	  tppara->P.x[22] = 0x4569;
	  tppara->P.x[21] = 0xe4ea;
	  tppara->P.x[20] = 0x5719;
	  tppara->P.x[19] = 0xe84f;
	  tppara->P.x[18] = 0x8c0f;
	  tppara->P.x[17] = 0xf6fe;
	  tppara->P.x[16] = 0xd8a7;
	  tppara->P.x[15] = 0x1875;
	  tppara->P.x[14] = 0xa892;
	  tppara->P.x[13] = 0x77c9;
	  tppara->P.x[12] = 0xb1d1;
	  tppara->P.x[11] = 0x6856;
	  tppara->P.x[10] = 0xaf9d;
	  tppara->P.x[9] = 0xbf95;
	  tppara->P.x[8] = 0x2f30;
	  tppara->P.x[7] = 0x3060;
	  tppara->P.x[6] = 0xe6e4;
	  tppara->P.x[5] = 0x6fcb;
	  tppara->P.x[4] = 0xc0c7;
	  tppara->P.x[3] = 0x5b72;
	  tppara->P.x[2] = 0xb9ad;
	  tppara->P.x[1] = 0x108b;
	  tppara->P.x[0] = 0x38db;

	  tppara->P.y[24] = 0x0000;
	  tppara->P.y[23] = 0x0001;
	  tppara->P.y[22] = 0xa895;
	  tppara->P.y[21] = 0xb2de;
	  tppara->P.y[20] = 0x8683;
	  tppara->P.y[19] = 0xb105;
	  tppara->P.y[18] = 0x7176;
	  tppara->P.y[17] = 0xb38c;
	  tppara->P.y[16] = 0xee52;
	  tppara->P.y[15] = 0x2e62;
	  tppara->P.y[14] = 0xabb3;
	  tppara->P.y[13] = 0xff33;
	  tppara->P.y[12] = 0xc345;
	  tppara->P.y[11] = 0xbef6;
	  tppara->P.y[10] = 0xe044;
	  tppara->P.y[9] = 0x1477;
	  tppara->P.y[8] = 0xa07b;
	  tppara->P.y[7] = 0xc8fb;
	  tppara->P.y[6] = 0x3d34;
	  tppara->P.y[5] = 0x9758;
	  tppara->P.y[4] = 0x11e0;
	  tppara->P.y[3] = 0x61a5;
	  tppara->P.y[2] = 0xaed5;
	  tppara->P.y[1] = 0x7c99;
	  tppara->P.y[0] = 0xb648;

	  // group order m
	  tppara->m[24] = 0x0000;
	  tppara->m[23] = 0x0000;
	  tppara->m[22] = 0x0000;
	  tppara->m[21] = 0x0000;
	  tppara->m[20] = 0x0000;
	  tppara->m[19] = 0x0000;
	  tppara->m[18] = 0x0000;
	  tppara->m[17] = 0x0000;
	  tppara->m[16] = 0x0000;
	  tppara->m[15] = 0x0000;
	  tppara->m[14] = 0x0000;
	  tppara->m[13] = 0x0000;
	  tppara->m[12] = 0x0000;
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
	  tppara->c[24] = 0x0000;
	  tppara->c[23] = 0x0000;
	  tppara->c[22] = 0x0000;
	  tppara->c[21] = 0x0000;
	  tppara->c[20] = 0x0000;
	  tppara->c[19] = 0x0000;
	  tppara->c[18] = 0x0000;
	  tppara->c[17] = 0x0000;
	  tppara->c[16] = 0x0000;
	  tppara->c[15] = 0x0000;
	  tppara->c[14] = 0x0001;
	  tppara->c[13] = 0x3fc1;
	  tppara->c[12] = 0xf689;
	  tppara->c[11] = 0xbf1f;
	  tppara->c[10] = 0x9dd2;
	  tppara->c[9] = 0xf6a0;
	  tppara->c[8] = 0x4502;
	  tppara->c[7] = 0x647c;
	  tppara->c[6] = 0xa57c;
	  tppara->c[5] = 0xb742;
	  tppara->c[4] = 0xdd8d;
	  tppara->c[3] = 0x8c58;
	  tppara->c[2] = 0x0268;
	  tppara->c[1] = 0x84eb;
	  tppara->c[0] = 0xf828;
#elif defined(THIRTYTWO_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[12] = 0x00000000;
	  tppara->p[11] = 0x9fe10043;
	  tppara->p[10] = 0xe769f5e5;
	  tppara->p[9] = 0xf9c76e5b;
	  tppara->p[8] = 0xb3525c50;
	  tppara->p[7] = 0x4e3761a2;
	  tppara->p[6] = 0x91e040db;
	  tppara->p[5] = 0xe3600054;
	  tppara->p[4] = 0xb9405afd;
	  tppara->p[3] = 0x9b835a83;
	  tppara->p[2] = 0x48bd2272;
	  tppara->p[1] = 0x73a7fd97;
	  tppara->p[0] = 0x7b1407d7;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  // point P
	  tppara->P.x[12] = 0x00000000;
	  tppara->P.x[11] = 0x087d4569;
	  tppara->P.x[10] = 0xe4ea5719;
	  tppara->P.x[9] = 0xe84f8c0f;
	  tppara->P.x[8] = 0xf6fed8a7;
	  tppara->P.x[7] = 0x1875a892;
	  tppara->P.x[6] = 0x77c9b1d1;
	  tppara->P.x[5] = 0x6856af9d;
	  tppara->P.x[4] = 0xbf952f30;
	  tppara->P.x[3] = 0x3060e6e4;
	  tppara->P.x[2] = 0x6fcbc0c7;
	  tppara->P.x[1] = 0x5b72b9ad;
	  tppara->P.x[0] = 0x108b38db;

	  tppara->P.y[12] = 0x00000000;
	  tppara->P.y[11] = 0x0001a895;
	  tppara->P.y[10] = 0xb2de8683;
	  tppara->P.y[9] = 0xb1057176;
	  tppara->P.y[8] = 0xb38cee52;
	  tppara->P.y[7] = 0x2e62abb3;
	  tppara->P.y[6] = 0xff33c345;
	  tppara->P.y[5] = 0xbef6e044;
	  tppara->P.y[4] = 0x1477a07b;
	  tppara->P.y[3] = 0xc8fb3d34;
	  tppara->P.y[2] = 0x975811e0;
	  tppara->P.y[1] = 0x61a5aed5;
	  tppara->P.y[0] = 0x7c99b648;

	  // group order m
	  tppara->m[12] = 0x00000000;
	  tppara->m[11] = 0x00000000;
	  tppara->m[10] = 0x00000000;
	  tppara->m[9] = 0x00000000;
	  tppara->m[8] = 0x00000000;
	  tppara->m[7] = 0x00000000;
	  tppara->m[6] = 0x00000000;
	  tppara->m[5] = 0x00000000;
	  tppara->m[4] = 0x800003ff;
	  tppara->m[3] = 0xffffffff;
	  tppara->m[2] = 0xffffffff;
	  tppara->m[1] = 0xffffffff;
	  tppara->m[0] = 0xffffffff;

	// c= ((p^(k-1))+1)/m
	  tppara->c[12] = 0x00000000;
	  tppara->c[11] = 0x00000000;
	  tppara->c[10] = 0x00000000;
	  tppara->c[9] = 0x00000000;
	  tppara->c[8] = 0x00000000;
	  tppara->c[7] = 0x00000001;
	  tppara->c[6] = 0x3fc1f689;
	  tppara->c[5] = 0xbf1f9dd2;
	  tppara->c[4] = 0xf6a04502;
	  tppara->c[3] = 0x647ca57c;
	  tppara->c[2] = 0xb742dd8d;
	  tppara->c[1] = 0x8c580268;
	  tppara->c[0] = 0x84ebf828;
	  
 #endif
  }

#endif
