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

includes NN2;

module NN2M {
  provides interface NN2;
  uses interface TPCurveParam;
  uses interface NN;
}

implementation { 

   //Computes a = (b.r*c.r - b.i*c.i) mod d + i*(b.i*c.r + b.r*c.i) mod d
   command void NN2.ModMult(NN2_NUMBER * a, NN2_NUMBER * b, NN2_NUMBER * c, NN_DIGIT * d, NN_UINT digits) {
     NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], t3[NUMWORDS];

     call NN.ModMult(t1,b->r,c->r,d,digits);
     call NN.ModMult(t2,b->i,c->i,d,digits);
     call NN.ModSub(t3,t1,t2,d,digits);
     call NN.ModMult(t1,b->i,c->r,d,digits);
     call NN.ModMult(t2,b->r,c->i,d,digits);
     call NN.ModAdd(t1,t1,t2,d,digits);
     call NN.Assign(a->r,t3,digits);
     call NN.Assign(a->i,t1,digits);
   }

   //Computes a = (b.r^2 - b.i^2) mod d + i*(2*b.i*b.r) mod d
   command void NN2.ModSqr(NN2_NUMBER * a, NN2_NUMBER * b, NN_DIGIT * d, NN_UINT digits) {
     NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], t3[NUMWORDS];

     call NN.ModSqr(t1, b->r, d, digits);
     call NN.ModSqr(t2, b->i, d, digits);
     call NN.ModSub(t3, t1, t2, d, digits);
     call NN.ModMult(t1, b->i, b->r, d, digits);
     call NN.Assign(a->r, t3, digits);
     call NN.Assign(a->i,t1,digits);
     call NN.ModAdd(a->i, a->i, t1, d, digits);
   }

   //assign a = b
   command void NN2.Assign(NN2_NUMBER * a,NN2_NUMBER * b,NN_UINT digits) {
     call NN.Assign(a->r,b->r,digits);
     call NN.Assign(a->i,b->i,digits);
   } 	  
}
