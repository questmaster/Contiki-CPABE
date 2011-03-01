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
 * module for TP
 *
 * Author: Panos Kampanakis
 * Date: 02/04/2005
 */

#include "TP.h"
#include <string.h>
#include <watchdog.h>

//#define PROJECTIVE
///#define PROJECTIVE_M
///#define FIXED_P

#ifndef TPSSC
#error *** Use one of the Pairing or CP-ABE curves! NOT the Secp ones. *** 
#endif

static TPParams tpparam;
static NN_DIGIT Qx[NUMWORDS], mQy[NUMWORDS];
#ifdef BARRETT_REDUCTION
static Barrett Bbuf;
#endif

#ifdef PROJECTIVE

#elif defined(PROJECTIVE_M)

#elif defined(FIXED_P)
static PointSlope *pPointSlope;
#else  //affine coordinate system

#endif
static NN_DIGIT inv2[NUMWORDS]; // used for lucas division


#if defined (CONTIKI_TARGET_IMOTE2)
  void print_val(NN_DIGIT *num) {
    int i;
    for (i=NUMWORDS-2; i>=0; i--) trace(DBG_USR1,"%08x",*(num+i));
    trace(DBG_USR1,"\n\r");
  }
#endif
#ifdef MICA
  void print_val(NN_DIGIT *num){
    int i;
    for(i=0; i<NUMWORDS; i++){
      printf("%02x", num[NUMWORDS-i-1]);
    }
    printf("\n\r");
  }
#endif


#ifdef PROJECTIVE
static void dbl_line_projective(NN2_NUMBER *u, Point *P0, NN_DIGIT *Z0, Point *P1, NN_DIGIT *Z1){
    NN_DIGIT t1[NUMWORDS];
    NN_DIGIT t2[NUMWORDS];
    NN_DIGIT t3[NUMWORDS];

    /*
    if(NNZero(Z1, NUMWORDS)){
      //infinity
      return;
    }
    */
    //t2=Z1^2
    NNModSqr(t2, Z1, tpparam.p, NUMWORDS);

    //t3=3*X1^2+a*Z1^4
    NNModSqr(t3, P1->x, tpparam.p, NUMWORDS);  //X1^2
    memcpy(t1, t3, NUMWORDS*NN_DIGIT_LEN);
    NNLShift(t1, t1, 1, NUMWORDS);  //2*X1^2
    NNModAdd(t3, t3, t1, tpparam.p, NUMWORDS);  //3*X1^2
    NNModSqr(t1, t2, tpparam.p, NUMWORDS);  //Z1^4
    if(tpparam.E.a_one == FALSE)
      NNModMult(t1, t1, tpparam.E.a, tpparam.p, NUMWORDS);  //a*Z1^4
    NNModAdd(t3, t3, t1, tpparam.p, NUMWORDS);


    //t1=Y1^2
    NNModSqr(t1, P1->y, tpparam.p, NUMWORDS);
    
    //Z3=2Y1*Z1
    NNModMult(Z0, P1->y, Z1, tpparam.p, NUMWORDS);
    NNLShift(Z0, Z0, 1, NUMWORDS);
    NNModSmall(Z0, tpparam.p, NUMWORDS);

    //g=Z3*t2*(-yQi) + (2*t1-t3*(t2*xQ+X1))
    NNModMult(u->i, Z0, t2, tpparam.p, NUMWORDS);  //Z3*t2
    NNModMult(u->i, u->i, mQy, tpparam.p, NUMWORDS);  //Z3*t2*(-yQ)
    NNModMult(t2, t2, Qx, tpparam.p, NUMWORDS);  //t2*xQ
    NNModAdd(t2, t2, P1->x, tpparam.p, NUMWORDS);  //t2*xQ+X1
    NNModMult(t2, t2, t3, tpparam.p, NUMWORDS);  //t3*(t2*xQ+X1)
    NNLShift(u->r, t1, 1, NUMWORDS);  //2*t1
    NNModSmall(u->r, tpparam.p, NUMWORDS);
    NNModSub(u->r, u->r, t2, tpparam.p, NUMWORDS);  //2*t1 - t3*(t2*xQ+X1)

    //t2=4*X1*t1
    NNModMult(t2, P1->x, t1, tpparam.p, NUMWORDS);  //X1*t1
    NNLShift(t2, t2, 2, NUMWORDS);
    NNModSmall(t2, tpparam.p, NUMWORDS);

    //X3=t3^2 - 2*t2 = t3^2 - t2 - t2
    NNModSqr(P0->x, t3, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
    //t1=8t1^2
    NNModSqr(t1, t1, tpparam.p, NUMWORDS);
    NNLShift(t1, t1, 3, NUMWORDS);
    NNModSmall(t1, tpparam.p, NUMWORDS);
    //Y3=t3(t2-X3)-t1
    NNModSub(P1->y, t2, P1->x, tpparam.p, NUMWORDS);
    NNModMult(P1->y, P1->y, t3, tpparam.p, NUMWORDS);
    NNModSub(P1->y, P1->y, t1, tpparam.p, NUMWORDS);

  }

static void add_line_projective(NN2_NUMBER *u, Point *P0, NN_DIGIT *Z0, Point *P1, NN_DIGIT *Z1, Point *P2){
    NN_DIGIT t1[NUMWORDS];
    NN_DIGIT t2[NUMWORDS];
    NN_DIGIT t3[NUMWORDS];
    NN_DIGIT t4[NUMWORDS];

    //what if infinity

    //t1=Z1^2
    NNModSqr(t1, Z1, tpparam.p, NUMWORDS);
    //t2=Z1*t1
    NNModMult(t2, Z1, t1, tpparam.p, NUMWORDS);
    //t3=X*t1
    NNModMult(t3, P2->x, t1, tpparam.p, NUMWORDS);
    //t1=Y*t2
    NNModMult(t1, P2->y, t2, tpparam.p, NUMWORDS);
    //t2=t3-X1
    NNModSub(t2, t3, P1->x, tpparam.p, NUMWORDS);
    //t3=t1-Y1
    NNModSub(t3, t1, P1->y, tpparam.p, NUMWORDS);
    //Z3=Z1*t2
    NNModMult(Z0, Z1, t2, tpparam.p, NUMWORDS);
    //g=Z3*(-yQi) + (Z3*Y - t3*(xQ + X))
    NNModMult(u->i, Z0, mQy, tpparam.p, NUMWORDS);  //Z3*yQi
    NNModAdd(t4, Qx, P2->x, tpparam.p, NUMWORDS);  //xQ + X
    NNModMult(u->r, t4, t3, tpparam.p, NUMWORDS);  //t3*(xQ+X)
    NNModMult(t4, Z0, P2->y, tpparam.p, NUMWORDS);  //Z3*Y
    NNModSub(u->r, t4, u->r, tpparam.p, NUMWORDS);  //Z3*Y - t3*(xQ + X))
    //t1=t2^2
    NNModSqr(t1, t2, tpparam.p, NUMWORDS);
    //t4=t2*t1
    NNModMult(t4, t2, t1, tpparam.p, NUMWORDS);
    //t2=X1*t1
    NNModMult(t2, P1->x, t1, tpparam.p, NUMWORDS);
    //X3=t3^2-(t4+2*t2)
    NNModSqr(P0->x, t3, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t4, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
    //Y3=t3*(t2-X3)-Y1*t4
    NNModSub(t1, t2, P0->x, tpparam.p, NUMWORDS);  //t2-X3
    NNModMult(t1, t3, t1, tpparam.p, NUMWORDS);  //t3*(t2-X3)
    NNModMult(P0->y, P1->y, t4, tpparam.p, NUMWORDS);  //Y1*t4
    NNModSub(P0->y, t1, P0->y, tpparam.p, NUMWORDS);  //t3*(t2-X3)-Y1*t4
  }

  //Miller's algorithm based on projective coordinate system
bool TP_Miller(NN2_NUMBER *ef, Point P){
    NN2_NUMBER temp1;
    Point V;
    int t;
    NN_DIGIT Z[NUMWORDS];

    memset(ef->r, 0, NUMWORDS*NN_DIGIT_LEN); // f = 1
    ef->r[0] = 0x1;
    memset(ef->i, 0, NUMWORDS*NN_DIGIT_LEN);
    //V = P; // V=P
   NNAssign(V.x, P.x, NUMWORDS);
    NNAssign(V.y, P.y, NUMWORDS);
    memset(Z, 0, NUMWORDS*NN_DIGIT_LEN);
    Z[0] = 0x1;

    t = (NNBits(tpparam.m,NUMWORDS))-2; // t=bits-2
    
    while (t>-1) {
	  watchdog_periodic();
      dbl_line_projective(&temp1, &V, Z, &V, Z);
      NN2ModSqr(ef, ef, tpparam.p, NUMWORDS);  //f=f^2
      NN2ModMult(ef, ef, &temp1, tpparam.p, NUMWORDS); // f=f*g

      if ((t>0) && (NNTestBit(tpparam.m,t))) {
	add_line_projective(&temp1, &V, Z, &V, Z, &(P));
	NN2ModMult(ef, ef, &temp1, tpparam.p, NUMWORDS);//f=f*g
      }
      t--;
    }
    return TRUE;
  }


#elif defined(PROJECTIVE_M)

static void dbl_line_projective_m(NN2_NUMBER *u, NN2_NUMBER *temp, Point *P0, NN_DIGIT *Z0, Point *P1, NN_DIGIT *Z1, int m){
    NN_DIGIT t1[NUMWORDS];
    NN_DIGIT t2[NUMWORDS];
    NN_DIGIT t3[NUMWORDS];
    NN_DIGIT W[NUMWORDS];
    int i;

    NNAssign(P0->x, P1->x, NUMWORDS);
    NNAssign(Z0, Z1, NUMWORDS);
    //Y1 = 2Y1
    NNLShift(P0->y, P1->y, 1, NUMWORDS);
    NNModSmall(P0->y, tpparam.p, NUMWORDS);
    //W = a*Z1^4
    NNModSqr(W, Z1, tpparam.p, NUMWORDS);
    NNModSqr(W, W, tpparam.p, NUMWORDS);
    NNModMult(W, W, tpparam.E.a, tpparam.p, NUMWORDS);

    for (i=m; i>=0; i--){
      //t1 = Y0^2
      NNModSqr(t1, P0->y, tpparam.p, NUMWORDS);
      //t3 = 3*X0^2 + W
      NNModSqr(t2, P0->x, tpparam.p, NUMWORDS);
      NNLShift(t3, t2, 1, NUMWORDS);
      NNModAdd(t3, t3, t2, tpparam.p, NUMWORDS);
      NNModAdd(t3, t3, W, tpparam.p, NUMWORDS);

      //t2 = Z0^2
      NNModSqr(t2, Z1, tpparam.p, NUMWORDS);
      //temp->r = t1/2 - t3*(t2*XQ + X0)
      NNAssign(temp->r, t1, NUMWORDS);
      if (t1[0] % 2 == 1)
	NNAdd(temp->r, temp->r, tpparam.p, NUMWORDS);
      NNRShift(temp->r, temp->r, 1, NUMWORDS);
      NNModMult(temp->i, t2, Qx, tpparam.p, NUMWORDS);  //t2*XQ
      NNModAdd(temp->i, temp->i, P0->x, tpparam.p, NUMWORDS);  //t2*XQ + X0
      NNModMult(temp->i, temp->i, t3, tpparam.p, NUMWORDS);  //t3*(t2*XQ + X0)
      NNModSub(temp->r, temp->r, temp->i, tpparam.p, NUMWORDS);
      //Z0 = Z0*Y0
      NNModMult(Z0, Z0, P0->y, tpparam.p, NUMWORDS);
      //temp->i = Z0*t2*(-YQ)
      NNModMult(temp->i, Z0, t2, tpparam.p, NUMWORDS);
      NNModMult(temp->i, temp->i, mQy, tpparam.p, NUMWORDS);
      //t2 = X0*t1
      NNModMult(t2, P0->x, t1, tpparam.p, NUMWORDS);
      //X0 = t3^2 - 2*t2
      NNModSqr(P0->x, t3, tpparam.p, NUMWORDS);
      NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
      NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
      //t1 = t1^2
      NNModSqr(t1, t1, tpparam.p, NUMWORDS);
      //Y0 = 2*t3(t2-X0)-t1^2
      NNModSub(P0->y, t2, P0->x, tpparam.p, NUMWORDS);
      NNModMult(P0->y, t3, P0->y, tpparam.p, NUMWORDS);
      NNLShift(P0->y, P0->y, 1, NUMWORDS);
      NNModSmall(P0->y, tpparam.p, NUMWORDS);
      NNModSub(P0->y, P0->y, t1, tpparam.p, NUMWORDS);
      if(m>0){
	NNModMult(W, W, t1, tpparam.p, NUMWORDS);
      }
      //f = f^2*g
      NN2ModSqr(u, u, tpparam.p, NUMWORDS);  //f=f^2
      NN2ModMult(u, u, temp, tpparam.p, NUMWORDS); // f=f*g      
    }
    //Y0 = Y0/2
    if (P0->y[0] % 2 == 1)
      NNAdd(P0->y, P0->y, tpparam.p, NUMWORDS);
    NNRShift(P0->y, P0->y, 1, NUMWORDS);
  }

static void add_line_projective(NN2_NUMBER *u, Point *P0, NN_DIGIT *Z0, Point *P1, NN_DIGIT *Z1, Point *P2){
    NN_DIGIT t1[NUMWORDS];
    NN_DIGIT t2[NUMWORDS];
    NN_DIGIT t3[NUMWORDS];
    NN_DIGIT t4[NUMWORDS];

    //what if infinity

    //t1=Z1^2
    NNModSqr(t1, Z1, tpparam.p, NUMWORDS);
    //t2=Z1*t1
    NNModMult(t2, Z1, t1, tpparam.p, NUMWORDS);
    //t3=X*t1
    NNModMult(t3, P2->x, t1, tpparam.p, NUMWORDS);
    //t1=Y*t2
    NNModMult(t1, P2->y, t2, tpparam.p, NUMWORDS);
    //t2=t3-X1
    NNModSub(t2, t3, P1->x, tpparam.p, NUMWORDS);
    //t3=t1-Y1
    NNModSub(t3, t1, P1->y, tpparam.p, NUMWORDS);
    //Z3=Z1*t2
    NNModMult(Z0, Z1, t2, tpparam.p, NUMWORDS);
    //g=Z3*(-yQi) + (Z3*Y - t3*(xQ + X))
    NNModMult(u->i, Z0, mQy, tpparam.p, NUMWORDS);  //Z3*yQi
    NNModAdd(t4, Qx, P2->x, tpparam.p, NUMWORDS);  //xQ + X
    NNModMult(u->r, t4, t3, tpparam.p, NUMWORDS);  //t3*(xQ+X)
    NNModMult(t4, Z0, P2->y, tpparam.p, NUMWORDS);  //Z3*Y
    NNModSub(u->r, t4, u->r, tpparam.p, NUMWORDS);  //Z3*Y - t3*(xQ + X))
    //t1=t2^2
    NNModSqr(t1, t2, tpparam.p, NUMWORDS);
    //t4=t2*t1
    NNModMult(t4, t2, t1, tpparam.p, NUMWORDS);
    //t2=X1*t1
    NNModMult(t2, P1->x, t1, tpparam.p, NUMWORDS);
    //X3=t3^2-(t4+2*t2)
    NNModSqr(P0->x, t3, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t4, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
    NNModSub(P0->x, P0->x, t2, tpparam.p, NUMWORDS);
    //Y3=t3*(t2-X3)-Y1*t4
    NNModSub(t1, t2, P0->x, tpparam.p, NUMWORDS);  //t2-X3
    NNModMult(t1, t3, t1, tpparam.p, NUMWORDS);  //t3*(t2-X3)
    NNModMult(P0->y, P1->y, t4, tpparam.p, NUMWORDS);  //Y1*t4
    NNModSub(P0->y, t1, P0->y, tpparam.p, NUMWORDS);  //t3*(t2-X3)-Y1*t4
  }

  //return the largest number of consecutive 0s, start is the position of first 0 bit
static int check_m_0(NN_DIGIT *a, int start){
    NN_DIGIT temp;
    int rest, original_rest;
    bool done = FALSE;
    int i, original_i;

    original_i = i = start / NN_DIGIT_BITS;
    temp = *(a+i);
    original_rest = rest = start % NN_DIGIT_BITS;
    while (!done && rest >= 0){
      if (temp & ((NN_DIGIT)1<<rest))
	done = TRUE;
      else
	rest--;
    }
    if (rest >= 0){  //1 in current digit
      return original_rest - rest;
    }
    i--;
    //no 1 in current digit after start
    while(*(a+i) == 0 && i >= 0){
      i--;
    }
    
    if (i < 0){  //all following digits are zero
      return (start + 1);
    }
    //find bit 1 in the following digit
    done = FALSE;
    rest = NN_DIGIT_BITS - 1;
    temp = *(a+i);
    while (!done){
      if (temp & ((NN_DIGIT)1<<rest))
	done = TRUE;
      else
	rest--;
    }
    return ((original_i - 1 - i) * NN_DIGIT_BITS + original_rest + NN_DIGIT_BITS - rest);
  }

bool TP_Miller(NN2_NUMBER *ef, Point P){
    NN2_NUMBER temp1;
    Point V;
    int t, m;
    NN_DIGIT Z[NUMWORDS];

    memset(ef->r, 0, NUMWORDS*NN_DIGIT_LEN); // f = 1
    ef->r[0] = 0x1;
    memset(ef->i, 0, NUMWORDS*NN_DIGIT_LEN);
    //V = P; // V=P
    NNAssign(V.x, P.x, NUMWORDS);
    NNAssign(V.y, P.y, NUMWORDS);
    memset(Z, 0, NUMWORDS*NN_DIGIT_LEN);
    Z[0] = 0x1;

    t = (NNBits(tpparam.m,NUMWORDS))-2; // t=bits-2
    
    while (t>-1) {
      //find the largest m consecutive 0 bits
      m = check_m_0(tpparam.m, t);
      dbl_line_projective_m(ef, &temp1, &V, Z, &V, Z, m);
      t = t - m;
      if (t>0) {
	add_line_projective(&temp1, &V, Z, &V, Z, &(P));
	NN2ModMult(ef, ef, &temp1, tpparam.p, NUMWORDS);//f=f*g
      }
      t--;
    }
    return TRUE;

  }

#elif defined(FIXED_P)

  // affine point doubleing, P0 = 2*P1, slope is set slope=[(3x1^2+a)/(2y1)]^2=(y{2P1}-y1)/(x{2P1}-x1)
  // P0 and P1 can be the same point
static void aff_dbl(PointSlope *pNode, Point * P0, Point * P1)
  {
   Point P;
   NN_DIGIT t1[NUMWORDS], t2[NUMWORDS];
   P=*P1;
   pNode->dbl = TRUE;
   
   NNModSqr(t1, P.x, tpparam.p, NUMWORDS); //x1^2
   NNLShift(t2, t1, 1, NUMWORDS);
   if(NNCmp(t2, tpparam.p, NUMWORDS) >= 0)
     NNSub(t2, t2, tpparam.p, NUMWORDS); //2x1^2
   NNModAdd(t2, t2, t1, tpparam.p, NUMWORDS); //3x1^2 
   NNModAdd(t1, t2, tpparam.E.a, tpparam.p, NUMWORDS); //3x1^2+a
   NNLShift(t2, P.y, 1, NUMWORDS);
   if(NNCmp(t2, tpparam.p, NUMWORDS) >= 0)
     NNSub(t2, t2, tpparam.p, NUMWORDS); //2y1
   NNModDiv(pNode->slope, t1, t2, tpparam.p, NUMWORDS); //(3x1^2+a)/(2y1) 
   NNModSqr(t1, pNode->slope, tpparam.p, NUMWORDS); //[(3x1^2+a)/(2y1)]^2
   NNLShift(t2, P.x, 1, NUMWORDS);
   if(NNCmp(t2, tpparam.p, NUMWORDS) >= 0)
     NNSub(t2, t2, tpparam.p, NUMWORDS); //2x1
   NNModSub(P0->x, t1, t2, tpparam.p, NUMWORDS); //P0.x = [(3x1^2+a)/(2y1)]^2 - 2x1
   NNModSub(t1, P.x, P0->x, tpparam.p, NUMWORDS); //x1-P0.x
   NNModMult(t2, pNode->slope, t1, tpparam.p, NUMWORDS); //[(3x1^2+a)/(2y1)](x1-P0.x)
   NNModSub(P0->y, t2, P.y, tpparam.p, NUMWORDS); //[(3x1^2+a)/(2y1)](x1-P0.x)-y1
   
   NNAssign(pNode->P.x, P.x, NUMWORDS);
   NNAssign(pNode->P.y, P.y, NUMWORDS);

  }

  // affine Point addition, P0 = P1 + P2, slope is set slope=(y2-y1)/(x2-x1)
  // P0, P1 and P2 can be the same point
static void aff_add(PointSlope *pNode, Point * P0, Point * P1, Point * P2)
  {
    NN_DIGIT t1[NUMWORDS], t2[NUMWORDS];
    Point Pt1, Pt2;
    Pt1 = *P1; Pt2 = *P2;
    pNode->dbl = FALSE;
    NNModSub(t1, Pt2.y, Pt1.y, tpparam.p, NUMWORDS); //y2-y1
    NNModSub(t2, Pt2.x, Pt1.x, tpparam.p, NUMWORDS); //y2-y1
    NNModDiv(pNode->slope, t1, t2, tpparam.p, NUMWORDS); //(y2-y1)/(x2-x1) 
    NNModSqr(t1, pNode->slope, tpparam.p, NUMWORDS); //[(y2-y1)/(x2-x1)]^2
    NNModSub(t2, t1, Pt1.x, tpparam.p, NUMWORDS); 
    NNModSub(P0->x, t2, Pt2.x, tpparam.p, NUMWORDS); //P0.x = [(y2-y1)/(x2-x1)]^2 - x1 - x2
    NNModSub(t1, Pt1.x, P0->x, tpparam.p, NUMWORDS); //x1-P0.x
    NNModMult(t2, t1, pNode->slope, tpparam.p, NUMWORDS); //(x1-P0.x)(y2-y1)/(x2-x1)
    NNModSub(P0->y, t2, Pt1.y, tpparam.p, NUMWORDS); //P0.y=(x1-P0.x)(y2-y1)/(x2-x1)-y1

    NNAssign(pNode->P.x, Pt1.x, NUMWORDS);
    NNAssign(pNode->P.y, Pt1.y, NUMWORDS);
  }

static void precompute(Point P){
    Point V;
    int t;
    bool first_bit = TRUE;
    PointSlope *current;

    V = P; // V=P
    t = (NNBits(tpparam.m,NUMWORDS))-2; // t=bits-2

    while (t>-1) {
      if (first_bit){
	pPointSlope = (PointSlope *)malloc(sizeof(PointSlope));
	current = pPointSlope;
	first_bit = FALSE;
      }else{
	current->next = (PointSlope *)malloc(sizeof(PointSlope));
	current = current->next;
      }
      current->next = NULL;
      aff_dbl(current, &V, &V); //V=2V
      if ((t>0) && (NNTestBit(tpparam.m,t))) {
	current->next = (PointSlope *)malloc(sizeof(PointSlope));
	current = current->next;
	current->next = NULL;
	aff_add(current, &V, &V, &(P)); //V=V+P
      }
      t--;
    }

  }

  // Miller's algorithm
bool TP_Miller(NN2_NUMBER *ef) { 
    NN2_NUMBER temp1;
    PointSlope *current;


    NNAssign(temp1.i,mQy,NUMWORDS);
    memset(ef->r, 0, NUMWORDS*NN_DIGIT_LEN); // f = 1
    ef->r[0] = 0x1;
    memset(ef->i, 0, NUMWORDS*NN_DIGIT_LEN);
    
    current = pPointSlope;
    while(current != NULL){
      NNModAdd(temp1.r, current->P.x, Qx, tpparam.p, NUMWORDS); //x+Qx
      NNModMult(temp1.r, current->slope, temp1.r, tpparam.p, NUMWORDS); //slope(x+Qx)
      NNModSub(temp1.r, current->P.y, temp1.r, tpparam.p, NUMWORDS); //y-slope(x+Qx)	
      if(current->dbl){
	NN2ModSqr(ef, ef, tpparam.p, NUMWORDS);
	NN2ModMult(ef, ef, &temp1, tpparam.p, NUMWORDS);
      }else{
	NN2ModMult(ef, ef, &temp1, tpparam.p, NUMWORDS);//f=f*g
      }
      current = current->next;

    }
    return TRUE;
  }
  
#else  //affine coordinate

  // affine point doubleing, P0 = 2*P1, slope is set slope=[(3x1^2+a)/(2y1)]^2=(y{2P1}-y1)/(x{2P1}-x1)
  // P0 and P1 can be the same point
static void aff_dbl(NN2_NUMBER *u, Point * P0, Point * P1)
  {
   Point P;
   NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], slope[NUMWORDS];
   P=*P1;
   
   NNModSqr(t1, P.x, tpparam.p, NUMWORDS); //x1^2
   NNLShift(t2, t1, 1, NUMWORDS);
   if(NNCmp(t2, tpparam.p, NUMWORDS) >= 0)
     NNSub(t2, t2, tpparam.p, NUMWORDS); //2x1^2
   NNModAdd(t2, t2, t1, tpparam.p, NUMWORDS); //3x1^2 
   NNModAdd(t1, t2, tpparam.E.a, tpparam.p, NUMWORDS); //3x1^2+a
   NNLShift(t2, P.y, 1, NUMWORDS);
   if(NNCmp(t2, tpparam.p, NUMWORDS) >= 0)
     NNSub(t2, t2, tpparam.p, NUMWORDS); //2y1
   NNModDiv(slope, t1, t2, tpparam.p, NUMWORDS); //(3x1^2+a)/(2y1) 
   NNModSqr(t1, slope, tpparam.p, NUMWORDS); //[(3x1^2+a)/(2y1)]^2
   NNLShift(t2, P.x, 1, NUMWORDS);
   if(NNCmp(t2, tpparam.p, NUMWORDS) >= 0)
     NNSub(t2, t2, tpparam.p, NUMWORDS); //2x1
   NNModSub(P0->x, t1, t2, tpparam.p, NUMWORDS); //P0.x = [(3x1^2+a)/(2y1)]^2 - 2x1
   NNModSub(t1, P.x, P0->x, tpparam.p, NUMWORDS); //x1-P0.x
   NNModMult(t2, slope, t1, tpparam.p, NUMWORDS); //[(3x1^2+a)/(2y1)](x1-P0.x)
   NNModSub(P0->y, t2, P.y, tpparam.p, NUMWORDS); //[(3x1^2+a)/(2y1)](x1-P0.x)-y1
   
   NNModAdd(u->r, P.x, Qx, tpparam.p, NUMWORDS); //x+Qx
   NNModMult(u->r, slope, u->r, tpparam.p, NUMWORDS); //slope(x+Qx)
   NNModSub(u->r, P.y, u->r, tpparam.p, NUMWORDS); //y-slope(x+Qx)


  }

  // affine Point addition, P0 = P1 + P2, slope is set slope=(y2-y1)/(x2-x1)
  // P0, P1 and P2 can be the same point
static void aff_add(NN2_NUMBER *u, Point * P0, Point * P1, Point * P2)
  {
    NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], slope[NUMWORDS];
    Point Pt1, Pt2;
    Pt1 = *P1; Pt2 = *P2;
    NNModSub(t1, Pt2.y, Pt1.y, tpparam.p, NUMWORDS); //y2-y1
    NNModSub(t2, Pt2.x, Pt1.x, tpparam.p, NUMWORDS); //y2-y1
    NNModDiv(slope, t1, t2, tpparam.p, NUMWORDS); //(y2-y1)/(x2-x1) 
    NNModSqr(t1, slope, tpparam.p, NUMWORDS); //[(y2-y1)/(x2-x1)]^2
    NNModSub(t2, t1, Pt1.x, tpparam.p, NUMWORDS); 
    NNModSub(P0->x, t2, Pt2.x, tpparam.p, NUMWORDS); //P0.x = [(y2-y1)/(x2-x1)]^2 - x1 - x2
    NNModSub(t1, Pt1.x, P0->x, tpparam.p, NUMWORDS); //x1-P0.x
    NNModMult(t2, t1, slope, tpparam.p, NUMWORDS); //(x1-P0.x)(y2-y1)/(x2-x1)
    NNModSub(P0->y, t2, Pt1.y, tpparam.p, NUMWORDS); //P0.y=(x1-P0.x)(y2-y1)/(x2-x1)-y1
    
    NNModAdd(u->r, Pt1.x, Qx, tpparam.p, NUMWORDS); //x+Qx
    NNModMult(u->r, slope, u->r, tpparam.p, NUMWORDS); //slope(x+Qx)
    NNModSub(u->r, Pt1.y, u->r, tpparam.p, NUMWORDS); //y-slope(x+Qx)

  }

  // Miller's algorithm
bool TP_Miller(NN2_NUMBER *ef, Point P) { 
    NN2_NUMBER temp1;
    Point V;
    int t;

    NNAssign(temp1.i,mQy,NUMWORDS);
    memset(ef->r, 0, NUMWORDS*NN_DIGIT_LEN); // f = 1
    ef->r[0] = 0x1;
    memset(ef->i, 0, NUMWORDS*NN_DIGIT_LEN);
    V = P; // V=P
    
    t = (NNBits(tpparam.m,NUMWORDS))-2; // t=bits-2
    
    while (t>-1) {
		watchdog_periodic();
      aff_dbl(&temp1, &V, &V); //V=2V
      NN2ModSqr(ef, ef, tpparam.p, NUMWORDS);
      NN2ModMult(ef, ef, &temp1, tpparam.p, NUMWORDS);
      
      if ((t>0) && (NNTestBit(tpparam.m,t))) {
	aff_add(&temp1, &V, &V, &(P)); //V=V+P
	NN2ModMult(ef, ef, &temp1, tpparam.p, NUMWORDS);//f=f*g
      }
      t--;

    }

    return TRUE;
  }

#endif
  
  // initialize the Pairing with the point to be used along with the private key
void TP_init(Point P, Point Q) {
    NN_DIGIT Qy[NUMWORDS], two[NUMWORDS];
    
    //ECC_tpinit();
    get_TP_param(&tpparam);
#ifdef BARRETT_REDUCTION
    NNModBarrettInit(tpparam.p, NUMWORDS, &Bbuf);
#endif
    //tpparam = ECC_get_tpparam();
    NNAssign(Qx,Q.x,NUMWORDS); 
    NNAssign(Qy,Q.y,NUMWORDS);
    NNModNeg(mQy,Qy,tpparam.p,NUMWORDS); //-Qy
    memset(inv2, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(two, 0, NUMWORDS*NN_DIGIT_LEN);
    two[0] = 0x02;
    NNModInv(inv2, two, tpparam.p, NUMWORDS); //2^(-1)

#ifdef FIXED_P
    //compute all intermediate nodes and slopes
    precompute(P);
#endif
    
  }

  
  // final exponentiation in Miller's algorithm
  // using the (u+iv)^(k-1) trick and Lucas exponentiation optimization
void TP_final_expon(NN2_NUMBER *r,NN2_NUMBER *ef) {
    NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], t3[NUMWORDS], two[NUMWORDS];
	NN2_NUMBER in;
    

#ifdef CPABE_DEBUG
	// TODO: DEBUG!
		int i;

		printf("tate-ef.r ");
		for (i = NUMWORDS-1; i >= 0; i--) {
	#ifndef THIRTYTWO_BIT_PROCESSOR
			printf("%x ",ef->r[i]);
	#else
			printf("%x %x ", (uint16_t)(ef->r[i] >> 16), (uint16_t) ef->r[i]);
	#endif
		}
		printf("\n");
		printf("tate-ef.i ");
		for (i = NUMWORDS-1; i >= 0; i--) {
	#ifndef THIRTYTWO_BIT_PROCESSOR
			printf("%x ",ef->i[i]);
	#else
			printf("%x %x ", (uint16_t)(ef->i[i] >> 16), (uint16_t) ef->i[i]);
	#endif
		}
		printf("\n");
/*
		ef->r[7] = 0x00000000;
		ef->r[6] = 0x24bc0fbc;
		ef->r[5] = 0x7cac3301;
		ef->r[4] = 0x8bcda4ee;
		ef->r[3] = 0xf87ba4a2;
		ef->r[2] = 0x65ca3930;
		ef->r[1] = 0x0b250f97;
		ef->r[0] = 0xae223ba5;
		ef->i[7] = 0x00000000;
		ef->i[6] = 0x05502f1a;
		ef->i[5] = 0xcdab354a;
		ef->i[4] = 0xc8d694dd;
		ef->i[3] = 0x19b05271;
		ef->i[2] = 0xbcdceb98;
		ef->i[1] = 0x8f5c4d32;
		ef->i[0] = 0x9a1d3fdc;
*/
	// TODO: DEBUG
#endif
	
	NNModSqr(t1, ef->r, tpparam.p, NUMWORDS); // x^2
    NNModSqr(t2, ef->i, tpparam.p, NUMWORDS); // y^2
    NNModAdd(t3, t1, t2, tpparam.p, NUMWORDS); // x^2+y^2
    NNModSub(t1, t1, t2, tpparam.p, NUMWORDS); // x^2-y^2
#if defined (CONTIKI_TARGET_IMOTE2) || defined (TARGET_LINUX32)
    NNModDiv(t1, t1, t3, tpparam.p, NUMWORDS); //(x^2-y^2)/(x^2+y^2)
#else
    NNModDivOpt(t1, t1, t3, tpparam.p, NUMWORDS);
#endif

	NNAssignDigit(two, 2, NUMWORDS);
	NNModMult(t2, ef->r, ef->i, tpparam.p, NUMWORDS); // x*y
	NNModMult(t2, t2, two, tpparam.p, NUMWORDS); // 2*x*y
#if defined (CONTIKI_TARGET_IMOTE2) || defined (TARGET_LINUX32)
	NNModDiv(t2, t2, t3, tpparam.p, NUMWORDS); // (2*x*y)/(x^2+y^2)
#else
	NNModDivOpt(t2, t2, t3, tpparam.p, NUMWORDS);
#endif
	NNModNeg(t2, t2, tpparam.p, NUMWORDS); // -((2*x*y)/(x^2+y^2))
	
	NNAssign(in.r, t1, NUMWORDS);
	NNAssign(in.i, t2, NUMWORDS);
	
#ifdef CPABE_DEBUG
	// TODO: DEBUG!

	printf("tate-in.r ");
	for (i = NUMWORDS-1; i >= 0; i--) {
#ifndef THIRTYTWO_BIT_PROCESSOR
		printf("%x ",in.r[i]);
#else
		printf("%x %x ", (uint16_t)(in.r[i] >> 16), (uint16_t) in.r[i]);
#endif
	}
	printf("\n");
	printf("tate-in.i ");
	for (i = NUMWORDS-1; i >= 0; i--) {
#ifndef THIRTYTWO_BIT_PROCESSOR
		printf("%x ",in.i[i]);
#else
		printf("%x %x ", (uint16_t)(in.i[i] >> 16), (uint16_t) in.i[i]);
#endif
	}
	printf("\n");
/*
	in.r[7] = 0x00000000;
	in.r[6] = 0x71aaacf6;
	in.r[5] = 0x715726c5;
	in.r[4] = 0x3ef4dc02;
	in.r[3] = 0x62c7b725;
	in.r[2] = 0x3e4014c7;
	in.r[1] = 0x8cd3bc8a;
	in.r[0] = 0x97effc7b;
	in.i[7] = 0x00000000;
	in.i[6] = 0x18595983;
	in.i[5] = 0x3a0892e2;
	in.i[4] = 0x9e95b3df;
	in.i[3] = 0x62f1517d;
	in.i[2] = 0xa4519319;
	in.i[1] = 0x9a872b8b;
	in.i[0] = 0xd3a5984b;
*/
	// TODO: DEBUG END
#endif
	
    NN2LucExp(r,&in,tpparam.c,inv2,tpparam.p,NUMWORDS); // Lucas exponentiation 
  }

  // Set the res value to be the Tate Pairing result
/*static void TP_computeTP(NN2_NUMBER *res, Point P) {
    NN2_NUMBER ef;
	
    TP_Miller(&ef, P);
    TP_final_expon(res,&ef);
	
  }
*/
void TP_TatePairing(NN2_NUMBER *res, Point P, Point Q) {
    NN2_NUMBER ef;

	TP_init(P, Q);
    TP_Miller(&ef, P);
    TP_final_expon(res,&ef);
}

TPParams *TP_getTPparams() {
	return &tpparam;
}
