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
 * Module for curve ss512k2
 *
 * Author: Panos Kampanakis
 * Date: 02/07/2007
 */

#define A_IS_ONE
includes ECC;

module ss512k2 {
  provides interface TPCurveParam;
  uses interface NN;
}

implementation {

  command void TPCurveParam.get_param(TPParams *tppara)
  {
  	#ifdef EIGHT_BIT_PROCESSOR 
  	  memset(tppara->p, 0, NUMWORDS*NN_DIGIT_LEN);
  	  tppara->p[64]=0x00;
  	  tppara->p[63]=0x80;
  	  tppara->p[46]=0x02;
  	  tppara->p[44]=0x01;
  	  tppara->p[43]=0x40;
  	  tppara->p[26]=0x01;
  	  tppara->p[23]=0x80;
	  tppara->p[20]=0x02;
  	  tppara->p[2]=0x08;
  	  tppara->p[0]=0x03;

	  // a = 1 
	  memset(tppara->E.a, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->E.a[0] =  0x01;
	  tppara->E.a_one = TRUE;
	  // b = 0
	  memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	  // point P
	  tppara->P.x[64]=0x00;
	  tppara->P.x[63]=0x36;
	  tppara->P.x[62]=0x09;
	  tppara->P.x[61]=0x4B;
	  tppara->P.x[60]=0x25;
	  tppara->P.x[59]=0x2E;
	  tppara->P.x[58]=0xF2;
	  tppara->P.x[57]=0xFE;
	  tppara->P.x[56]=0x15;
	  tppara->P.x[55]=0x98;
	  tppara->P.x[54]=0x81;
	  tppara->P.x[53]=0x43;
	  tppara->P.x[52]=0x5E;
	  tppara->P.x[51]=0x65;
	  tppara->P.x[50]=0xE8;
	  tppara->P.x[49]=0x43;
	  tppara->P.x[48]=0xE2;
	  tppara->P.x[47]=0x22;
	  tppara->P.x[46]=0x39;
	  tppara->P.x[45]=0x63;
	  tppara->P.x[44]=0x47;
	  tppara->P.x[43]=0xDB;
	  tppara->P.x[42]=0x6C;
	  tppara->P.x[41]=0x5D;
	  tppara->P.x[40]=0x9B;
	  tppara->P.x[39]=0x36;
	  tppara->P.x[38]=0xD4;
	  tppara->P.x[37]=0xAD;
	  tppara->P.x[36]=0xE8;
	  tppara->P.x[35]=0xBA;
	  tppara->P.x[34]=0x79;
	  tppara->P.x[33]=0x19;
	  tppara->P.x[32]=0xF4;
	  tppara->P.x[31]=0x62;
	  tppara->P.x[30]=0x33;
	  tppara->P.x[29]=0xBA;
	  tppara->P.x[28]=0x3E;
	  tppara->P.x[27]=0x39;
	  tppara->P.x[26]=0x33;
	  tppara->P.x[25]=0x1E;
	  tppara->P.x[24]=0xF8;
	  tppara->P.x[23]=0x40;
	  tppara->P.x[22]=0x4B;
	  tppara->P.x[21]=0xE6;
	  tppara->P.x[20]=0x83;
	  tppara->P.x[19]=0x8A;
	  tppara->P.x[18]=0xBF;
	  tppara->P.x[17]=0x12;
	  tppara->P.x[16]=0xD7;
	  tppara->P.x[15]=0x4B;
	  tppara->P.x[14]=0x90;
	  tppara->P.x[13]=0x46;
	  tppara->P.x[12]=0x22;
	  tppara->P.x[11]=0x98;
	  tppara->P.x[10]=0xD3;
	  tppara->P.x[9]=0x8C;
	  tppara->P.x[8]=0x17;
	  tppara->P.x[7]=0x50;
	  tppara->P.x[6]=0x39;
	  tppara->P.x[5]=0x8B;
	  tppara->P.x[4]=0x01;
	  tppara->P.x[3]=0xAE;
	  tppara->P.x[2]=0x7C;
	  tppara->P.x[1]=0x68;
	  tppara->P.x[0]=0xDF;

	  tppara->P.y[64]=0x00;
	  tppara->P.y[63]=0x64;
	  tppara->P.y[62]=0x67;
	  tppara->P.y[61]=0x3F;
	  tppara->P.y[60]=0x0F;
	  tppara->P.y[59]=0x2E;
	  tppara->P.y[58]=0x8F;
	  tppara->P.y[57]=0x6D;
	  tppara->P.y[56]=0xD2;
	  tppara->P.y[55]=0xE3;
	  tppara->P.y[54]=0x20;
	  tppara->P.y[53]=0x65;
	  tppara->P.y[52]=0x58;
	  tppara->P.y[51]=0xFB;
	  tppara->P.y[50]=0xBA;
	  tppara->P.y[49]=0x8C;
	  tppara->P.y[48]=0x1E;
	  tppara->P.y[47]=0x80;
	  tppara->P.y[46]=0x31;
	  tppara->P.y[45]=0x11;
	  tppara->P.y[44]=0xFE;
	  tppara->P.y[43]=0xA0;
	  tppara->P.y[42]=0x0E;
	  tppara->P.y[41]=0xAE;
	  tppara->P.y[40]=0xB7;
	  tppara->P.y[39]=0x55;
	  tppara->P.y[38]=0x2C;
	  tppara->P.y[37]=0x7C;
	  tppara->P.y[36]=0x8A;
	  tppara->P.y[35]=0xEC;
	  tppara->P.y[34]=0x37;
	  tppara->P.y[33]=0xE7;
	  tppara->P.y[32]=0x97;
	  tppara->P.y[31]=0x1E;
	  tppara->P.y[30]=0x26;
	  tppara->P.y[29]=0x7E;
	  tppara->P.y[28]=0xC5;
	  tppara->P.y[27]=0x45;
	  tppara->P.y[26]=0x55;
	  tppara->P.y[25]=0xE8;
	  tppara->P.y[24]=0xE3;
	  tppara->P.y[23]=0xFE;
	  tppara->P.y[22]=0xA7;
	  tppara->P.y[21]=0x50;
	  tppara->P.y[20]=0xB4;
	  tppara->P.y[19]=0xB8;
	  tppara->P.y[18]=0xF1;
	  tppara->P.y[17]=0x56;
	  tppara->P.y[16]=0x1F;
	  tppara->P.y[15]=0x36;
	  tppara->P.y[14]=0x36;
	  tppara->P.y[13]=0x6E;
	  tppara->P.y[12]=0xAE;
	  tppara->P.y[11]=0xF5;
	  tppara->P.y[10]=0xA6;
	  tppara->P.y[9]=0x14;
	  tppara->P.y[8]=0x28;
	  tppara->P.y[7]=0x5C;
	  tppara->P.y[6]=0x90;
	  tppara->P.y[5]=0x22;
	  tppara->P.y[4]=0x53;
	  tppara->P.y[3]=0x75;
	  tppara->P.y[2]=0x74;
	  tppara->P.y[1]=0xAE;
	  tppara->P.y[0]=0xEF;
	  //printf("y initialized\n\r");	    
	  // c= p^k-1/m
	  memset(tppara->c, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->c[44]=0x01;
	  tppara->c[23]=0x80;
	  tppara->c[0]=0x04;
  	  
  	  // group order m
	  memset(tppara->m, 0, NUMWORDS*NN_DIGIT_LEN);
	  tppara->m[19]=0x80;
	  tppara->m[2]=0x02;
	  tppara->m[0]=0x01;	

  	#endif
  	
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
	    tppara->E.a_one = TRUE;
	    // b = 0
	    memset(tppara->E.b, 0, NUMWORDS*NN_DIGIT_LEN);

	    // point P
	    tppara->P.x[16]=0x00000000;
	    tppara->P.x[15]=0x36094B25;
	    tppara->P.x[14]=0x2EF2FE15;
	    tppara->P.x[13]=0x9881435E;
	    tppara->P.x[12]=0x65E843E2;
	    tppara->P.x[11]=0x22396347;
	    tppara->P.x[10]=0xDB6C5D9B;
	    tppara->P.x[9]=0x36D4ADE8;
	    tppara->P.x[8]=0xBA7919F4;
	    tppara->P.x[7]=0x6233BA3E;
	    tppara->P.x[6]=0x39331EF8;
	    tppara->P.x[5]=0x404BE683;
	    tppara->P.x[4]=0x8ABF12D7;
	    tppara->P.x[3]=0x4B904622;
	    tppara->P.x[2]=0x98D38C17;
	    tppara->P.x[1]=0x50398B01;
	    tppara->P.x[0]=0xAE7C68DF;

	    tppara->P.y[16]=0x00000000;
	    tppara->P.y[15]=0x64673F0F;
	    tppara->P.y[14]=0x2E8F6DD2;
	    tppara->P.y[13]=0xE3206558;
	    tppara->P.y[12]=0xFBBA8C1E;
	    tppara->P.y[11]=0x803111FE;
	    tppara->P.y[10]=0xA00EAEB7;
	    tppara->P.y[9]=0x552C7C8A;
	    tppara->P.y[8]=0xEC37E797;
	    tppara->P.y[7]=0x1E267EC5;
	    tppara->P.y[6]=0x4555E8E3;
	    tppara->P.y[5]=0xFEA750B4;
	    tppara->P.y[4]=0xB8F1561F;
	    tppara->P.y[3]=0x36366EAE;
	    tppara->P.y[2]=0xF5A61428;
	    tppara->P.y[1]=0x5C902253;
	    tppara->P.y[0]=0x7574AEEF;
	    
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
