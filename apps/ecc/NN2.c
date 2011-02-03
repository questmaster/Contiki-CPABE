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
 * module NN2M, provides interface NN2
 * 
 * Implement well known optimized algorithms for Big Natural Complex Numbers
 *
 * Author: Panos Kampanakis
 * Date: 02/04/2007
 */

#include "NN2.h"
#include <string.h>

	//generadts random NN2_NUMBER
void NN2ModRandom(NN2_NUMBER * a, NN_DIGIT * b, NN_UINT digits) {
	NNModRandom(a->r, b, digits);
	NNModRandom(a->i, b, digits);
}

   //Computes a = (b.r*c.r - b.i*c.i) mod d + i*(b.i*c.r + b.r*c.i) mod d
void NN2ModMult(NN2_NUMBER * a, NN2_NUMBER * b, NN2_NUMBER * c, NN_DIGIT * d, NN_UINT digits) {
     NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], t3[NUMWORDS];

     NNModMult(t1,b->r,c->r,d,digits);
     NNModMult(t2,b->i,c->i,d,digits);
     NNModSub(t3,t1,t2,d,digits);
     NNModMult(t1,b->i,c->r,d,digits);
     NNModMult(t2,b->r,c->i,d,digits);
     NNModAdd(t1,t1,t2,d,digits);
     NNAssign(a->r,t3,digits);
     NNAssign(a->i,t1,digits);
   }

   //Computes a = (b.r^2 - b.i^2) mod d + i*(2*b.i*b.r) mod d
void NN2ModSqr(NN2_NUMBER * a, NN2_NUMBER * b, NN_DIGIT * d, NN_UINT digits) {
     NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], t3[NUMWORDS];

     NNModSqr(t1, b->r, d, digits);
     NNModSqr(t2, b->i, d, digits);
     NNModSub(t3, t1, t2, d, digits);
     NNModMult(t1, b->i, b->r, d, digits);
     NNAssign(a->r, t3, digits);
     NNAssign(a->i,t1,digits);
     NNModAdd(a->i, a->i, t1, d, digits);
   }

   //Computes a = b^c mod d; 
void NN2ModExp(NN2_NUMBER * a, NN2_NUMBER * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits) {
	int16_t i;
	NN2_NUMBER res;
	
	// Find first '1'
	i = NNBits(c, digits) - 2; // First index with a bit, skip first bit
	
	// Init algorithm
	NN2Assign(&res, b, digits);  // res = b;
	
	// Start Square&Multiply
	for (; i >= 0; i--) {
		if (NNTestBit(c, i)) { // 1
			NN2ModSqr(&res, &res, d, digits);
			NN2ModMult(&res, &res, b, d, digits);
		} else { // 0
			NN2ModSqr(&res, &res, d, digits);
		}
	}
	
	NN2Assign(a, &res, digits);
}

   //assign a = b
void NN2Assign(NN2_NUMBER * a,NN2_NUMBER * b,NN_UINT digits) {
     NNAssign(a->r,b->r,digits);
     NNAssign(a->i,b->i,digits);
   } 	  

   //assign a = (b+i0)
void NN2AssignNN(NN2_NUMBER * a,NN_DIGIT * b,NN_UINT digits) {
     NNAssign(a->r,b,digits);
     NNAssignZero(a->i,digits);
   } 	
   
   //assign a = (b.r, -b.i) mod c
void NN2ModInv(NN2_NUMBER * a,NN2_NUMBER * b,NN_DIGIT * c,NN_UINT digits) {
	NNAssign(a->r, b->r, digits);
	NNModNeg(a->i, b->i, c, digits);
}

// Returns the e{k-1}(b) Lucas function
static void NN2_Lucas (NN2_NUMBER * a,NN2_NUMBER * b, NN_DIGIT * k,NN_DIGIT * d,NN_UINT digits) {
	int i;
	NN_DIGIT two[NUMWORDS], k2[NUMWORDS];
	NN_DIGIT tmp[NUMWORDS], alpha[NUMWORDS];
	
	memset(two, 0, NUMWORDS*NN_DIGIT_LEN);
	two[0]= 0x1;
	NNSub(k2,k,two,digits);
	two[0]= 0x2;
	
	memset(alpha, 0, NUMWORDS*NN_DIGIT_LEN);
	alpha[0]=0x2;
	NNAssign(a->r,b->r,digits); //beta=b
	
	i = (NNBits(k2,NUMWORDS))-1;
	
	while (i>-1) {
		if (NNTestBit(k2,i)) {
			NNModMult(alpha,alpha,a->r,d,digits); 
			NNModSub(alpha,alpha,b->r,d,digits); //a=a*beta-P
			NNModSqr(a->r, a->r, d, digits);
			NNModSub(a->r,a->r,two,d,digits); //beta=beta^2-2
		}
		
		else {
			NNModMult(a->r,alpha,a->r,d,digits);
			NNModSub(a->r,a->r,b->r,d,digits); //beta=a*beta-P
			NNModSqr(alpha, alpha, d, digits);
			NNModSub(alpha,alpha,two,d,digits); //a=a^2-2
		}
		i--;
	}
	
	NNModMult(tmp, a->r, b->r,d,digits);
	NNModMult(alpha, alpha, two,d,digits);
	NNModSub(alpha, alpha, tmp,d,digits);
	
	NNModSqr(tmp, b->r,d,digits);
	NNModSub(tmp, tmp, two,d,digits);
	NNModSub(tmp, tmp, two,d,digits);
	NNModDivOpt(alpha, alpha, tmp,d,digits);
	
	NNAssign(a->i, alpha, digits); // was: NUMWORDS

}

// Return the lucas exponentatiation for the Tate Pairing lucas(2*b,k)/2
bool NN2LucExp(NN2_NUMBER * a,NN2_NUMBER * b, NN_DIGIT * k,NN_DIGIT * inv2,NN_DIGIT * d,NN_UINT digits) {
	
    NN2_NUMBER  temp1, temp2;
	/*    NN_DIGIT two[NUMWORDS];
	 memset(two, 0, NUMWORDS*NN_DIGIT_LEN);
	 two[0] = 0x02;
	 
	 NN_ModMult(temp1,two,b,d,digits); // 2b */
	// 2b it is faster with shif than the above line
	NNLShift(temp1.r, b->r, 1, digits);
	if(NNCmp(temp1.r, d, digits) >= 0)
		NNSub(temp1.r, temp1.r, d, digits);
	
	NN2_Lucas(&temp2,&temp1,k,d,digits); // lucas(2b,k)
	NNModMult(a->r,temp2.r,inv2,d,digits); // lucas(2b,k)*2^(-1)
	NNModMult(a->i, temp2.i, b->i,d,digits);
	
	return TRUE;
}


