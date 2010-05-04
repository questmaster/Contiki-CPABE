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
 * Module for curve ss512k2s
 *
 * Author: Panos Kampanakis
 * Date: 02/07/2007
 */

includes ECC;

module ss512k2s {
  provides interface TPCurveParam;
  uses interface NN;
}

implementation {

  command void TPCurveParam.get_param(TPParams *tppara)
  {
	#ifdef THIRTYTWO_BIT_PROCESSOR
	    //init parameters
	    //prime p
	    tppara->p[16]=0x00000000;
	    tppara->p[15]=0x80000000;
	    tppara->p[14]=0x00000000;
	    tppara->p[13]=0x00000000;
	    tppara->p[12]=0x00000000;
	    tppara->p[11]=0x00020001;
	    tppara->p[10]=0x40000000;
	    tppara->p[9]=0x00000000;
	    tppara->p[8]=0x00000000;
	    tppara->p[7]=0x00000000;
	    tppara->p[6]=0x00010000;
	    tppara->p[5]=0x80000002;
	    tppara->p[4]=0x00000000;
	    tppara->p[3]=0x00000000;
	    tppara->p[2]=0x00000000;
	    tppara->p[1]=0x00000000;
	    tppara->p[0]=0x00080003;

	    // a = 1 
	    memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	    tppara->E.a[0] =  0x00000001;
	    // b = 0
	    memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	    // point sP
	    // sPx
	    tppara->P.x[16]=0x00000000;
	    tppara->P.x[15]=0x187bdc81;
	    tppara->P.x[14]=0x33bbe39c;
	    tppara->P.x[13]=0x88dc3e91;
	    tppara->P.x[12]=0xe7d474eb;
	    tppara->P.x[11]=0x2a63422c;
	    tppara->P.x[10]=0x24cc1a0d;
	    tppara->P.x[9]=0xcd79d659;
	    tppara->P.x[8]=0xae154c07;
	    tppara->P.x[7]=0x88f135e6;
	    tppara->P.x[6]=0xe57ed851;
	    tppara->P.x[5]=0xc30767c7;
	    tppara->P.x[4]=0xc332a8e2;
	    tppara->P.x[3]=0x715a7293;
	    tppara->P.x[2]=0xd5dc13e1;
	    tppara->P.x[1]=0x629d73c1;
	    tppara->P.x[0]=0xd65ce363;
	    // sPy
	    tppara->P.y[16]=0x00000000;
	    tppara->P.y[15]=0x63a46b41;
	    tppara->P.y[14]=0xf6b86db1;
	    tppara->P.y[13]=0xfff56cc5;
	    tppara->P.y[12]=0xea70fd05;
	    tppara->P.y[11]=0x04bd2873;
	    tppara->P.y[10]=0xbea3ddc1;
	    tppara->P.y[9]=0x32fca2fd;
	    tppara->P.y[8]=0x628fb33c;
	    tppara->P.y[7]=0x8bc61dc9;
	    tppara->P.y[6]=0x83e6da28;
	    tppara->P.y[5]=0xdaac4c3c;
	    tppara->P.y[4]=0xbdfd8f20;
	    tppara->P.y[3]=0xe75a3407;
	    tppara->P.y[2]=0x5092ae3f;
	    tppara->P.y[1]=0xa544e2dd;
	    tppara->P.y[0]=0x9b3ff064;
	    
	    // group order m
	    memset(tppara->m, 0, NUMWORDS*NN_DIGIT_LEN);
	    tppara->m[4]=0x80000000;
	    tppara->m[0]=0x00020001;

	    // c= p^k-1/m
	    memset(tppara->c, 0, NUMWORDS*NN_DIGIT_LEN);
	    tppara->c[11]=0x00000001;
	    tppara->c[5]=0x80000000;
	    tppara->c[0]=0x00000004;
	#endif
  }
}
