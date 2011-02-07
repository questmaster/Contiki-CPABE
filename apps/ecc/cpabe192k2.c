/**
 * All new code in this distribution is Copyright 2007 by North Carolina
 * State University. All rights reserved. Redistribution and use in
 * source and binary forms are permitted provided that this entire
 * copyright notice is duplicated in all such copies, and that any
 * documentation, announcements, and other materials related to such
 * distribution and use acknowledge that the software was developed at
 * North Carolina State University, Raleigh, NC. No charge may be made
 * for copies, derivations, or distributions of this material without the
 * express written consent of the copyright holder. Neither the name of
 * the University nor the name of the author may be used to endorse or
 * promote products derived from this material without specific prior
 * written permission.
 *
 * IN NO EVENT SHALL THE NORTH CAROLINA STATE UNIVERSITY BE LIABLE TO ANY
 * PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE NORTH CAROLINA STATE UNIVERSITY HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN
 * "AS IS" BASIS, AND THE NORTH CAROLINA STATE UNIVERSITY HAS NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS. "
 *
 */

/**
 * Module for curve cpabe192k2
 *
 * Author: Daniel Jacobi
 * Date: 02/04/2010
 */

#include "ECC.h"
#include "NN.h"
#include <string.h>

/*
type a
q 790632772539640114822851543225224013723201237291862561643
h 4646921649987490668
r 170141188531071632644604909702696927233
exp2 127
exp1 102
sign1 1
sign0 1

 The Numbers:
 q dec: 790632772539640114822851543225224013723201237291862561643	q == p
 q hex: 203e975f82957167a4e4db0000000000407d2e9ec693936b
 
 h dec: 4646921649987490668											h == c
 h hex: 407d2e9ec693936c
 
 r dec: 170141188531071632644604909702696927233						r == m
 r hex: 80000040000000000000000000000001

Done.
 */

#ifdef CPABE192K2

void get_TP_param(TPParams *tppara)
  {
#ifdef EIGHT_BIT_PROCESSOR
// TODO: 8-bit
	  
#elif defined(SIXTEEN_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[12] = 0x0000;
	  tppara->p[11] = 0x203e;
	  tppara->p[10] = 0x975f;
	  tppara->p[9] = 0x8295;
	  tppara->p[8] = 0x7167;
	  tppara->p[7] = 0xa4e4;
	  tppara->p[6] = 0xdb00;
	  tppara->p[5] = 0x0000;
	  tppara->p[4] = 0x0000;
	  tppara->p[3] = 0x407d;
	  tppara->p[2] = 0x2e9e;
	  tppara->p[1] = 0xc693;
	  tppara->p[0] = 0x936b;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  
	  // point P
	  tppara->P.x[12] = 0x0000;
	  tppara->P.x[11] = 0x14a7;
	  tppara->P.x[10] = 0xe9bd;
	  tppara->P.x[9] = 0xf77a;
	  tppara->P.x[8] = 0x1e18;
	  tppara->P.x[7] = 0xd1e1;
	  tppara->P.x[6] = 0x0e06;
	  tppara->P.x[5] = 0x6b63;
	  tppara->P.x[4] = 0x0044;
	  tppara->P.x[3] = 0x030b;
	  tppara->P.x[2] = 0x830b;
	  tppara->P.x[1] = 0x8b37;
	  tppara->P.x[0] = 0x4869;

	  tppara->P.y[12] = 0x0000;
	  tppara->P.y[11] = 0x1492;
	  tppara->P.y[10] = 0x070c;
	  tppara->P.y[9] = 0x797c;
	  tppara->P.y[8] = 0xee4b;
	  tppara->P.y[7] = 0x75eb;
	  tppara->P.y[6] = 0x5244;
	  tppara->P.y[5] = 0xda95;
	  tppara->P.y[4] = 0xc61b;
	  tppara->P.y[3] = 0x3caf;
	  tppara->P.y[2] = 0xd18a;
	  tppara->P.y[1] = 0xb204;
	  tppara->P.y[0] = 0xd054;
	  
	  // group order m
	  memset(tppara->m, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->m[7] = 0x8000;
	  tppara->m[6] = 0x0040;
	  tppara->m[0] = 0x0001;
	  
	  // c= ((p^(k-1))+1)/m
	  memset(tppara->c, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->c[3] = 0x407d;
	  tppara->c[2] = 0x2e9e;
	  tppara->c[1] = 0xc693;
	  tppara->c[0] = 0x936c;
	  
#elif defined(THIRTYTWO_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[6] = 0x00000000;
	  tppara->p[5] = 0x203e975f;
	  tppara->p[4] = 0x82957167;
	  tppara->p[3] = 0xa4e4db00;
	  tppara->p[2] = 0x00000000;
	  tppara->p[1] = 0x407d2e9e;
	  tppara->p[0] = 0xc693936b;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x00000001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);
	  
	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;
	  
	  
	  // point P
	  tppara->P.x[6] = 0x00000000;
	  tppara->P.x[5] = 0x14a7e9bd;
	  tppara->P.x[4] = 0xf77a1e18;
	  tppara->P.x[3] = 0xd1e10e06;
	  tppara->P.x[2] = 0x6b630044;
	  tppara->P.x[1] = 0x030b830b;
	  tppara->P.x[0] = 0x8b374869;
	  
	  tppara->P.y[6] = 0x00000000;
	  tppara->P.y[5] = 0x1492070c;
	  tppara->P.y[4] = 0x797cee4b;
	  tppara->P.y[3] = 0x75eb5244;
	  tppara->P.y[2] = 0xda95c61b;
	  tppara->P.y[1] = 0x3cafd18a;
	  tppara->P.y[0] = 0xb204d054;
	  
	  // group order m
	  memset(tppara->m, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->m[3] = 0x80000040;
	  tppara->m[0] = 0x00000001;
	  
	  // c= ((p^(k-1))+1)/m
	  memset(tppara->c, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->c[1] = 0x407d2e9e;
	  tppara->c[0] = 0xc693936c;
	  
#endif
  }

#endif
