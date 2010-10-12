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
 * Module for curve ss192k2s
 *
 * Author: Panos Kampanakis
 * Date: 02/04/2007
 */

#include <ECC.h>
#include <NN.h>

// TODO: 8-bit

void get_TP_param(TPParams *tppara)
  {
#ifdef EIGHT_BIT_PROCESSOR
	  
#elif defined(SIXTEEN_BIT_PROCESSOR)
	  //init parameters
	  //prime p
	  tppara->p[12] = 0x0000;
	  tppara->p[11] = 0xF769;
	  tppara->p[10] = 0x064B;
	  tppara->p[9] = 0x0993;
	  tppara->p[8] = 0x8DE6;
	  tppara->p[7] = 0xAE19;
	  tppara->p[6] = 0x39DE;
	  tppara->p[5] = 0xCBC9;
	  tppara->p[4] = 0x775F;
	  tppara->p[3] = 0x9042;
	  tppara->p[2] = 0x27C7;
	  tppara->p[1] = 0xA95E;
	  tppara->p[0] = 0x9D63;
	  
	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x0001;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  tppara->E.a_minus3 = FALSE;
	  tppara->E.a_zero = FALSE;
	  tppara->E.a_one = TRUE;

	  // point sP
	  // sPx
	  tppara->P.x[12]=0x0000;
	  tppara->P.x[11]=0x12aa;
	  tppara->P.x[10]=0x8ba2;
	  tppara->P.x[9]=0x4f83;
	  tppara->P.x[8]=0x35b6;
	  tppara->P.x[7]=0x706d;
	  tppara->P.x[6]=0xc282;
	  tppara->P.x[5]=0xc720;
	  tppara->P.x[4]=0x6bf2;
	  tppara->P.x[3]=0x2bfa;
	  tppara->P.x[2]=0x68be;
	  tppara->P.x[1]=0x2333;
	  tppara->P.x[0]=0x6496;
	  // sPy
	  tppara->P.y[12]=0x0000;
	  tppara->P.y[11]=0xab40;
	  tppara->P.y[10]=0x9d39;
	  tppara->P.y[9]=0xffc2;
	  tppara->P.y[8]=0x54c1;
	  tppara->P.y[7]=0x3eb9;
	  tppara->P.y[6]=0xa426;
	  tppara->P.y[5]=0x051b;
	  tppara->P.y[4]=0xf3c4;
	  tppara->P.y[3]=0x3928;
	  tppara->P.y[2]=0xbcd9;
	  tppara->P.y[1]=0xb82b;
	  tppara->P.y[0]=0x1d36;
	  
	  // group order m
	  memset(tppara->m, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->m[3] = 0x1000;
	  tppara->m[2] = 0x0000;
	  tppara->m[1] = 0x0000;
	  tppara->m[0] = 0x0021;
	  
	  // c= p^k-1/m
	  tppara->c[12] = 0x0000;
	  tppara->c[11] = 0x0000;
	  tppara->c[10] = 0x0000;
	  tppara->c[9] = 0x0000;
	  tppara->c[8] = 0x000f;
	  tppara->c[7] = 0x7690;
	  tppara->c[6] = 0x64b0;
	  tppara->c[5] = 0x9938;
	  tppara->c[4] = 0xbe86;
	  tppara->c[3] = 0x57c3;
	  tppara->c[2] = 0xf1b0;
	  tppara->c[1] = 0xb78e;
	  tppara->c[0] = 0x80e4;
	  
#elif defined(THIRTYTWO_BIT_PROCESSOR)
	    //init parameters
	    //prime p
	    tppara->p[6] = 0x00000000;
	    tppara->p[5] = 0xF769064B;
	    tppara->p[4] = 0x09938DE6;
	    tppara->p[3] = 0xAE1939DE;
	    tppara->p[2] = 0xCBC9775F;
	    tppara->p[1] = 0x904227C7;
	    tppara->p[0] = 0xA95E9D63;

	    // a = 1 
	    memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	    tppara->E.a[0] =  0x00000001;
	    // b = 0
	    memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	    // point sP
	    // sPx
	    tppara->P.x[6]=0x00000000;
	    tppara->P.x[5]=0x12aa8ba2;
	    tppara->P.x[4]=0x4f8335b6;
	    tppara->P.x[3]=0x706dc282;
	    tppara->P.x[2]=0xc7206bf2;
	    tppara->P.x[1]=0x2bfa68be;
	    tppara->P.x[0]=0x23336496;
	    // sPy
	    tppara->P.y[6]=0x00000000;
	    tppara->P.y[5]=0xab409d39;
	    tppara->P.y[4]=0xffc254c1;
	    tppara->P.y[3]=0x3eb9a426;
	    tppara->P.y[2]=0x051bf3c4;
	    tppara->P.y[1]=0x3928bcd9;
	    tppara->P.y[0]=0xb82b1d36;
	    
	    // group order m
	    memset(tppara->m, 0, NUMWORDS*NN_DIGIT_LEN);
	    tppara->m[1] = 0x10000000;
	    tppara->m[0] = 0x00000021;

	    // c= p^k-1/m
	    tppara->c[6] = 0x00000000;
	    tppara->c[5] = 0x00000000;
	    tppara->c[4] = 0x0000000f;
	    tppara->c[3] = 0x769064b0;
	    tppara->c[2] = 0x9938be86;
	    tppara->c[1] = 0x57c3f1b0;
	    tppara->c[0] = 0xb78e80e4;
	#endif
  }

