/**
 * All new code in this distribution is Copyright 2005 by North Carolina
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
 * $Id: ECCM.nc,v 1.8 2007/11/02 22:36:39 aliu3 Exp $
 * ECCM, the module implement ECC.nc
 */
#include <ECC.h>
#include <string.h>
#include <dev/watchdog.h>
#include <lib/rand.h>
#ifdef TPSSC // used by ECC_compY
#include "TPCurveParam.h"
#endif
  //parameters for ECC operations
static Params param;

#ifdef BARRETT_REDUCTION
static  Barrett Bbuf;
#endif


#ifdef SLIDING_WIN
  //precomputed array for base point
static  Point pBaseArray[NUM_POINTS];
  //masks for sliding window method
static  NN_DIGIT mask[NUM_MASKS];
#endif		

static void c_add_projective(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1, Point * P2, NN_DIGIT * Z2);
#ifdef ADD_MIX
static void c_add_mix(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1, Point * P2);
#endif
static void c_dbl_projective(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1);


  // test whether the ith bit in a is one
static NN_DIGIT b_testbit(NN_DIGIT * a, int16_t i)
  {
    return (*(a + (i / NN_DIGIT_BITS)) & ((NN_DIGIT)1 << (i % NN_DIGIT_BITS)));
  }
  
  // set P0's x and y to zero
  static void p_clear(Point * P0)
  {
    NNAssignZero(P0->x, NUMWORDS);
    NNAssignZero(P0->y, NUMWORDS);
  }

  // P0 = P1
  static void p_copy(Point * P0, Point * P1)
  {
    NNAssign(P0->x, P1->x, NUMWORDS);
    NNAssign(P0->y, P1->y, NUMWORDS);
  }

  // test whether x and y of P0 is all zero
  static bool p_iszero(Point * P0)
  {
    bool result = FALSE;
    
    if (NNZero(P0->x, NUMWORDS))
      if (NNZero(P0->y, NUMWORDS))
        result = TRUE;
    return result;
  }

  // test whether points P1 and P2 are equal
  static bool p_equal(Point * P1, Point * P2)
  {
    if (NNEqual(P1->x, P2->x, NUMWORDS))
      if (NNEqual(P1->y, P2->y, NUMWORDS))
        return TRUE;
    return FALSE;
  }

  // test whether Z is one
  static bool Z_is_one(NN_DIGIT *Z)
  {
    uint8_t i;
    
    for (i = 1; i < NUMWORDS; i++)
      if (Z[i])
        return FALSE;
    if (Z[0] == 1)
      return TRUE;
    
    return FALSE;
  }

  // Point addition, P0 = P1 + P2
  static void c_add(Point * P0, Point * P1, Point * P2)
  {
    NN_DIGIT Z0[NUMWORDS];
    NN_DIGIT Z1[NUMWORDS];
    NN_DIGIT Z2[NUMWORDS];
    
    p_clear(P0);
    NNAssignZero(Z0, NUMWORDS);
    NNAssignZero(Z1, NUMWORDS);
    NNAssignZero(Z2, NUMWORDS);
    Z1[0] = 0x01;
    Z2[0] = 0x01;

#ifdef ADD_MIX
    c_add_mix(P0, Z0, P1, Z1, P2);
#else
    c_add_projective(P0, Z0, P1, Z1, P2, Z2);
#endif

    if (!Z_is_one(Z0))
    {
      NNModInv(Z1, Z0, param.p, NUMWORDS);
      NNModMultOpt(Z0, Z1, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->x, P0->x, Z0, param.p, param.omega, NUMWORDS);
      NNModMultOpt(Z0, Z0, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->y, P0->y, Z0, param.p, param.omega, NUMWORDS);
    }

  }

#ifdef ADD_MIX
  static void c_add_mix(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1, Point * P2)
  {
    NN_DIGIT t1[NUMWORDS];
    NN_DIGIT t2[NUMWORDS];
    NN_DIGIT t3[NUMWORDS];
    NN_DIGIT t4[NUMWORDS];
    NN_DIGIT Z2[NUMWORDS];

    //P2 == infinity
    if (NNZero(P2->x, NUMWORDS)){
      if (NNZero(P2->y, NUMWORDS)){
	p_copy(P0, P1);
	NNAssign(Z0, Z1, NUMWORDS);
	return;
      }
    }
    
    //P1 == infinity
    if (NNZero(Z1, NUMWORDS)){
      p_copy(P0, P2);
      NNAssignDigit(Z0, 1, NUMWORDS);
      return;
    }
    //T1 = Z1^2
    NNModSqrOpt(t1, Z1, param.p, param.omega, NUMWORDS);
    //T2 = T1*Z1
    NNModMultOpt(t2, t1, Z1, param.p, param.omega, NUMWORDS);
    //T1 = T1*P2->x
    NNModMultOpt(t1, t1, P2->x, param.p, param.omega, NUMWORDS);
    //T2 = T2*P2->y
    NNModMultOpt(t2, t2, P2->y, param.p, param.omega, NUMWORDS);
    //T1 = T1-P1->x
    NNModSub(t1, t1, P1->x, param.p, NUMWORDS);
    //T2 = T2-P1->y
    NNModSub(t2, t2, P1->y, param.p, NUMWORDS);

    if (NNZero(t1, NUMWORDS)){
      if (NNZero(t2, NUMWORDS)){
	NNAssignDigit(Z2, 1, NUMWORDS);
	c_dbl_projective(P0, Z0, P2, Z2);
	return;
      }else{
	NNAssignDigit(Z0, 0, NUMWORDS);
	return;
      }
    }
    //Z3 = Z1*T1
    NNModMultOpt(Z0, Z1, t1, param.p, param.omega, NUMWORDS);
    //T3 = T1^2
    NNModSqrOpt(t3, t1, param.p, param.omega, NUMWORDS);
    //T4 = T3*T1
    NNModMultOpt(t4, t3, t1, param.p, param.omega, NUMWORDS);
    //T3 = T3*P1->x
    NNModMultOpt(t3, t3, P1->x, param.p, param.omega, NUMWORDS);
    //T1 = 2*T3
    NNLShift(t1, t3, 1, NUMWORDS);
    NNModSmall(t1, param.p, NUMWORDS);
    //P0->x = T2^2
    NNModSqrOpt(P0->x, t2, param.p, param.omega, NUMWORDS);
    //P0->x = P0->x-T1
    NNModSub(P0->x, P0->x, t1, param.p, NUMWORDS);
    //P0->x = P0->x-T4
    NNModSub(P0->x, P0->x, t4, param.p, NUMWORDS);
    //T3 = T3-P0->x
    NNModSub(t3, t3, P0->x, param.p, NUMWORDS);
    //T3 = T3*T2
    NNModMultOpt(t3, t3, t2, param.p, param.omega, NUMWORDS);
    //T4 = T4*P1->y
    NNModMultOpt(t4, t4, P1->y, param.p, param.omega, NUMWORDS);
    //P0->y = T3-T4
    NNModSub(P0->y, t3, t4, param.p, NUMWORDS);

    return;
  }
#endif

  // (P0,Z0) = (P1,Z1) + (P2,Z2)  in Jacobian projective coordinate
  // P0, P1, P2 can be same pointer
  static void c_add_projective(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1, Point * P2, NN_DIGIT * Z2)
  {
    NN_DIGIT n0[NUMWORDS];
    NN_DIGIT n1[NUMWORDS];
    NN_DIGIT n2[NUMWORDS];
    NN_DIGIT n3[NUMWORDS];
    NN_DIGIT n4[NUMWORDS];
    NN_DIGIT n5[NUMWORDS];
    NN_DIGIT n6[NUMWORDS];

    if (NNZero(Z1, NUMWORDS))
    {
      p_copy(P0, P2);
      NNAssign(Z0, Z2, NUMWORDS);
      return;
    }

    if (NNZero(Z2, NUMWORDS))
    {
      p_copy(P0, P1);
      NNAssign(Z0, Z1, NUMWORDS);
      return;
    }
    
    //double
    if (p_equal(P1, P2))
    {
      c_dbl_projective(P0, Z0, P1, Z1);
      return;
    }
    
    //add_proj
    //n1, n2
    if (Z_is_one(Z2))
    {
      // n1 = P1->x, n2 = P1->y
      NNAssign(n1, P1->x, NUMWORDS);
      NNAssign(n2, P1->y, NUMWORDS);
    }
    else
    {
      //n1 = P1->x * Z2^2
      NNModSqrOpt(n0, Z2, param.p, param.omega, NUMWORDS);
      NNModMultOpt(n1, P1->x, n0, param.p, param.omega, NUMWORDS);
      //n2 = P1->y * Z2^3
      NNModMultOpt(n0, n0, Z2, param.p, param.omega, NUMWORDS);
      NNModMultOpt(n2, P1->y, n0, param.p, param.omega, NUMWORDS);
    }
    
    // n3, n4
    if (Z_is_one(Z1))
    {
      // n3 = P2->x, n4 = P2->y
      NNAssign(n3, P2->x, NUMWORDS);
      NNAssign(n4, P2->y, NUMWORDS);
    }
    else
    {
      // n3 = P2->x * Z1^2
      NNModSqrOpt(n0, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(n3, P2->x, n0, param.p, param.omega, NUMWORDS);
      // n4 = P2->y * Z1^3
      NNModMultOpt(n0, n0, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(n4, P2->y, n0, param.p, param.omega, NUMWORDS);
    }
    
    // n5 = n1 - n3, n6 = n2 - n4
    NNModSub(n5, n1, n3, param.p, NUMWORDS);
    NNModSub(n6, n2, n4, param.p, NUMWORDS);
    
    if (NNZero(n5, NUMWORDS)) {
      if (NNZero(n6, NUMWORDS))
      {
        // P1 and P2 are same point
        c_dbl_projective(P0, Z0, P1, Z1);
	return;
      }
      else
      {
        // P1 is the inverse of P2
        NNAssignZero(Z0, NUMWORDS);
        return;
      }
	}
    
    // 'n7' = n1 + n3, 'n8' = n2 + n4
    NNModAdd(n1, n1, n3, param.p, NUMWORDS);
    NNModAdd(n2, n2, n4, param.p, NUMWORDS);
    
    // Z0 = Z1 * Z2 * n5
    if (Z_is_one(Z1) && Z_is_one(Z2))
    {
      NNAssign(Z0, n5, NUMWORDS);
    }
    else
    {
      if (Z_is_one(Z1))
	NNAssign(n0, Z2, NUMWORDS);
      else if (Z_is_one(Z2))
	NNAssign(n0, Z1, NUMWORDS);
      else
	NNModMultOpt(n0, Z1, Z2, param.p, param.omega, NUMWORDS);
	  
      NNModMultOpt(Z0, n0, n5, param.p, param.omega, NUMWORDS);
    }
    
    // P0->x = n6^2 - n5^2 * 'n7'
    NNModSqrOpt(n0, n6, param.p, param.omega, NUMWORDS);
    NNModSqrOpt(n4, n5, param.p, param.omega, NUMWORDS);
    NNModMultOpt(n3, n1, n4, param.p, param.omega, NUMWORDS);
    NNModSub(P0->x, n0, n3, param.p, NUMWORDS);
	
    // 'n9' = n5^2 * 'n7' - 2 * P0->x
    NNLShift(n0, P0->x, 1, NUMWORDS);
    NNModSmall(n0, param.p, NUMWORDS);
    NNModSub(n0, n3, n0, param.p, NUMWORDS);
	
    // P0->y = (n6 * 'n9' - 'n8' * 'n5^3') / 2
    NNModMultOpt(n0, n0, n6, param.p, param.omega, NUMWORDS);
    NNModMultOpt(n5, n4, n5, param.p, param.omega, NUMWORDS);
    NNModMultOpt(n1, n2, n5, param.p, param.omega, NUMWORDS);
    NNModSub(n0, n0, n1, param.p, NUMWORDS);
	
    if ((n0[0] % 2) == 1)
      NNAdd(n0, n0, param.p, NUMWORDS);
	
    NNRShift(P0->y, n0, 1, NUMWORDS);
   
  }


  // (P0,Z0) = 2*(P1,Z1)
  // P0 and P1 can be same pointer
  static void c_dbl_projective(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1)
  {
    NN_DIGIT n0[NUMWORDS];
    NN_DIGIT n1[NUMWORDS];
    NN_DIGIT n2[NUMWORDS];
    NN_DIGIT n3[NUMWORDS];

    if (NNZero(Z1, NUMWORDS))
    {
      NNAssignZero(Z0, NUMWORDS);
      return;
    }

    // n1
    if (Z_is_one(Z1))
    {
      // n1 = 3 * P1->x^2 + param.E.a
      NNModSqrOpt(n0, P1->x, param.p, param.omega, NUMWORDS);
      NNLShift(n1, n0, 1, NUMWORDS);
      NNModSmall(n1, param.p, NUMWORDS);
      NNModAdd(n0, n0, n1, param.p, NUMWORDS);
      NNModAdd(n1, n0, param.E.a, param.p, NUMWORDS);
    }
    else
    {
      if (param.E.a_minus3)
      {
        //for a = -3
	// n1 = 3 * (X1 + Z1^2) * (X1 - Z1^2) = 3 * X1^2 - 3 * Z1^4
	NNModSqrOpt(n1, Z1, param.p, param.omega, NUMWORDS);
	NNModAdd(n0, P1->x, n1, param.p, NUMWORDS);
	NNModSub(n2, P1->x, n1, param.p, NUMWORDS);
	NNModMultOpt(n1, n0, n2, param.p, param.omega, NUMWORDS);
	NNLShift(n0, n1, 1, NUMWORDS);
	NNModSmall(n0, param.p, NUMWORDS);
	NNModAdd(n1, n0, n1, param.p, NUMWORDS);
      }
      else if (param.E.a_zero)
      {
	// n1 = 3 * P1->x^2
	NNModSqrOpt(n0, P1->x, param.p, param.omega, NUMWORDS);
	NNLShift(n1, n0, 1, NUMWORDS);
	NNModSmall(n1, param.p, NUMWORDS);
	NNModAdd(n1, n0, n1, param.p, NUMWORDS);
      }
      else
      {
	// n1 = 3 * P1->x^2 + param.E.a * Z1^4
	NNModSqrOpt(n0, P1->x, param.p, param.omega, NUMWORDS);
	NNLShift(n1, n0, 1, NUMWORDS);
	NNModSmall(n1, param.p, NUMWORDS);
	NNModAdd(n0, n0, n1, param.p, NUMWORDS);
	NNModSqrOpt(n1, Z1, param.p, param.omega, NUMWORDS);
	NNModSqrOpt(n1, n1, param.p, param.omega, NUMWORDS);
	NNModMultOpt(n1, n1, param.E.a, param.p, param.omega, NUMWORDS);
	NNModAdd(n1, n1, n0, param.p, NUMWORDS);
      }
    }

    // Z0 = 2 * P1->y * Z1
    if (Z_is_one(Z1))
    {
      NNAssign(n0, P1->y, NUMWORDS);
    }
    else
    {
      NNModMultOpt(n0, P1->y, Z1, param.p, param.omega, NUMWORDS);
    }
    NNLShift(Z0, n0, 1, NUMWORDS);
    NNModSmall(Z0, param.p, NUMWORDS);

    // n2 = 4 * P1->x * P1->y^2
    NNModSqrOpt(n3, P1->y, param.p, param.omega, NUMWORDS);
    NNModMultOpt(n2, P1->x, n3, param.p, param.omega, NUMWORDS);
    NNLShift(n2, n2, 2, NUMWORDS);
    NNModSmall(n2, param.p, NUMWORDS);

    // P0->x = n1^2 - 2 * n2
    NNLShift(n0, n2, 1, NUMWORDS);
    NNModSmall(n0, param.p, NUMWORDS);
    NNModSqrOpt(P0->x, n1, param.p, param.omega, NUMWORDS);
    NNModSub(P0->x, P0->x, n0, param.p, NUMWORDS);

    // n3 = 8 * P1->y^4
    NNModSqrOpt(n0, n3, param.p, param.omega, NUMWORDS);
    NNLShift(n3, n0, 3, NUMWORDS);
    NNModSmall(n3, param.p, NUMWORDS);

    // P0->y = n1 * (n2 - P0->x) - n3
    NNModSub(n0, n2, P0->x, param.p, NUMWORDS);
    NNModMultOpt(n0, n1, n0, param.p, param.omega, NUMWORDS);
    NNModSub(P0->y, n0, n3, param.p, NUMWORDS);

  }

#ifdef REPEAT_DOUBLE
  //m repeated point doublings (Algorithm 3.23 in "Guide to ECC")
  static void c_m_dbl_projective(Point * P0, NN_DIGIT *Z0, uint8_t m){
    uint8_t i;
    NN_DIGIT W[NUMWORDS];
    NN_DIGIT A[NUMWORDS];
    NN_DIGIT B[NUMWORDS];
    NN_DIGIT t1[NUMWORDS];
    NN_DIGIT y2[NUMWORDS];
    
    if (NNZero(Z0, NUMWORDS)){
      return;
    }

    //P0->y = 2*P0->y
    NNLShift(P0->y, P0->y, 1, NUMWORDS);
    NNModSmall(P0->y, param.p, NUMWORDS);
    //W = Z^4
    NNModSqrOpt(W, Z0, param.p, param.omega, NUMWORDS);
    NNModSqrOpt(W, W, param.p, param.omega, NUMWORDS);
    
    for (i=0; i<m; i++){
      if (param.E.a_minus3){
	//A = 3(X^2-W)
	NNModSqrOpt(A, P0->x, param.p, param.omega, NUMWORDS);
	NNModSub(A, A, W, param.p, NUMWORDS);
	NNLShift(t1, A, 1, NUMWORDS);
	NNModSmall(t1, param.p, NUMWORDS);
	NNModAdd(A, A, t1, param.p, NUMWORDS);
      }else if (param.E.a_zero){
	//A = 3*X^2
	NNModSqrOpt(t1, P0->x, param.p, param.omega, NUMWORDS);
	NNLShift(A, t1, 1, NUMWORDS);
	NNModSmall(A, param.p, NUMWORDS);
	NNModAdd(A, A, t1, param.p, NUMWORDS);
      }else{
	//A = 3*X^2 + a*W
	NNModSqrOpt(t1, P0->x, param.p, param.omega, NUMWORDS);
	NNLShift(A, t1, 1, NUMWORDS);
	NNModSmall(A, param.p, NUMWORDS);
	NNModAdd(A, A, t1, param.p, NUMWORDS);
	NNModMultOpt(t1, param.E.a, W, param.p, param.omega, NUMWORDS);
	NNModAdd(A, A, t1, param.p, NUMWORDS);
      }
      //B = X*Y^2
      NNModSqrOpt(y2, P0->y, param.p, param.omega, NUMWORDS);
      NNModMultOpt(B, P0->x, y2, param.p, param.omega, NUMWORDS);
      //X = A^2 - 2B
      NNModSqrOpt(P0->x, A, param.p, param.omega, NUMWORDS);
      NNLShift(t1, B, 1, NUMWORDS);
      NNModSmall(t1, param.p, NUMWORDS);
      NNModSub(P0->x, P0->x, t1, param.p, NUMWORDS);
      //Z = Z*Y
      NNModMultOpt(Z0, Z0, P0->y, param.p, param.omega, NUMWORDS);
      NNModSqrOpt(y2, y2, param.p, param.omega, NUMWORDS);
      if (i < m-1){
	//W = W*Y^4
	NNModMultOpt(W, W, y2, param.p, param.omega, NUMWORDS);
      }
      //Y = 2A(B-X)-Y^4
      NNLShift(A, A, 1, NUMWORDS);
      NNModSmall(A, param.p, NUMWORDS);
      NNModSub(B, B, P0->x, param.p, NUMWORDS);
      NNModMultOpt(A, A, B, param.p, param.omega, NUMWORDS);
      NNModSub(P0->y, A, y2, param.p, NUMWORDS);
    }
    if ((P0->y[0] % 2) == 1)
      NNAdd(P0->y, P0->y, param.p, NUMWORDS);
    NNRShift(P0->y, P0->y, 1, NUMWORDS);
  }
#endif

#ifdef AFFINE
  //P0 = 2*P1, P0 and P1 can be same point
  static void c_dbl_affine(Point *P0, Point *P1){
    NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], slope[NUMWORDS];
    
    if(NNZero(P1->y, NUMWORDS))
      return;
    NNModSqrOpt(t1, P1->x, param.p, param.omega, NUMWORDS); //x1^2
    NNLShift(t2, t1, 1, NUMWORDS);
    if(NNCmp(t2, param.p, NUMWORDS) >= 0)
      NNSub(t2, t2, param.p, NUMWORDS); //2*x1^2
    NNModAdd(t2, t2, t1, param.p, NUMWORDS); //3*x1^2 
    NNModAdd(t1, t2, param.E.a, param.p, NUMWORDS); //t1 = 3*x1^2+a
    NNLShift(t2, P1->y, 1, NUMWORDS);
    if(NNCmp(t2, param.p, NUMWORDS) >= 0)
     NNSub(t2, t2, param.p, NUMWORDS); //t2 = 2*y1
    NNModDiv(slope, t1, t2, param.p, NUMWORDS); //(3x1^2+a)/(2y1) 
    NNModSqrOpt(t1, slope, param.p, param.omega, NUMWORDS); //[(3x1^2+a)/(2y1)]^2
    NNLShift(t2, P1->x, 1, NUMWORDS);
    if(NNCmp(t2, param.p, NUMWORDS) >= 0)
      NNSub(t2, t2, param.p, NUMWORDS); //2*x1
    NNModSub(t1, t1, t2, param.p, NUMWORDS); //t1 = P0.x = [(3x1^2+a)/(2y1)]^2 - 2x1
    NNModSub(t2, P1->x, t1, param.p, NUMWORDS); //x1-P0.x
    NNModMultOpt(t2, slope, t2, param.p, param.omega, NUMWORDS); //[(3x1^2+a)/(2y1)](x1-P0.x)
    NNModSub(P0->y, t2, P1->y, param.p, NUMWORDS); //[(3x1^2+a)/(2y1)](x1-P0.x)-y1
    NNAssign(P0->x, t1, NUMWORDS);
  }

  //P0 = P1 + P2, P0 and P1 can be same point
  static void c_add_affine(Point *P0, Point *P1, Point *P2){
    NN_DIGIT t1[NUMWORDS], t2[NUMWORDS], slope[NUMWORDS];

    if (p_equal(P1, P2)){
      c_dbl_affine(P0, P1);
      return;
    }
    if (p_iszero(P1)){
      p_copy(P0, P2);
      return;
    }else if(p_iszero(P2)){
      p_copy(P0, P1);
      return;
    }
    NNModSub(t1, P2->y, P1->y, param.p, NUMWORDS); //y2-y1
    NNModSub(t2, P2->x, P1->x, param.p, NUMWORDS); //x2-x1
    NNModDiv(slope, t1, t2, param.p, NUMWORDS); //(y2-y1)/(x2-x1) 
    NNModSqrOpt(t1, slope, param.p, param.omega, NUMWORDS); //[(y2-y1)/(x2-x1)]^2
    NNModSub(t2, t1, P1->x, param.p, NUMWORDS); 
    NNModSub(t1, t2, P2->x, param.p, NUMWORDS); //P0.x = [(y2-y1)/(x2-x1)]^2 - x1 - x2
    NNModSub(t2, P1->x, t1, param.p, NUMWORDS); //x1-P0.x
    NNModMultOpt(t2, t2, slope, param.p, param.omega, NUMWORDS); //(x1-P0.x)(y2-y1)/(x2-x1)
    NNModSub(P0->y, t2, P1->y, param.p, NUMWORDS); //P0.y=(x1-P0.x)(y2-y1)/(x2-x1)-y1
    NNAssign(P0->x, t1, NUMWORDS);
  }
#endif


  //initialize parameters for ECC module
void ECC_init()
  {
    // get parameters -> done extern. 
	// First get param pointer and set curve parameters, then call this init!
//    get_param(&param);

#ifdef BARRETT_REDUCTION
    NNModBarrettInit(param.p, NUMWORDS, &Bbuf);
#endif

#ifdef SLIDING_WIN
    //precompute array for base point
    ECC_win_precompute(&(param.G), pBaseArray);
#endif
  }


#ifdef BARRETT_REDUCTION
  void ECC_BarrettSetBuf(){
    NNBarrettSetBuf(&Bbuf);
  }
#endif
  
  int ECC_point2octet(uint8_t *octet, NN_UINT octet_len, Point *P, bool compress){

    if (compress){
      if(octet_len < KEYDIGITS*NN_DIGIT_LEN+1){
	//too small octet
	return -1;
      }else{
	//compressed point representation
	if((1 & P->y[0]) == 0){
	  octet[0] = 0x02;
	}else{
	  octet[0] = 0x03;
	}
	NNEncode(octet+1, KEYDIGITS*NN_DIGIT_LEN, P->x, KEYDIGITS);
	return KEYDIGITS*NN_DIGIT_LEN+1;
      }
    }else{//non compressed
      if(octet_len < 2*KEYDIGITS*NN_DIGIT_LEN+1){
	return -1;
      }else{
	octet[0] = 0x04;
	NNEncode(octet+1, KEYDIGITS*NN_DIGIT_LEN, P->x, KEYDIGITS);
	NNEncode(octet+1+KEYDIGITS*NN_DIGIT_LEN, KEYDIGITS*NN_DIGIT_LEN, P->y, KEYDIGITS);
	return 2*KEYDIGITS*NN_DIGIT_LEN+1;
      }
    }
  }

  int ECC_octet2point(Point *P, uint8_t *octet, int octet_len){
    NN_DIGIT alpha[NUMWORDS], tmp[NUMWORDS];

    if (octet[0] == 0){//infinity
      NNAssignZero(P->x, NUMWORDS);
      NNAssignZero(P->y, NUMWORDS);
    }else if (octet[0] == 4){//non compressed
      NNDecode(P->x, NUMWORDS, octet+1, KEYDIGITS*NN_DIGIT_LEN);
      NNDecode(P->y, NUMWORDS, octet+1+KEYDIGITS*NN_DIGIT_LEN, KEYDIGITS*NN_DIGIT_LEN);
      return 2*KEYDIGITS*NN_DIGIT_LEN+1;
    }else if (octet[0] == 2 || octet[0] == 3){//compressed form
      NNDecode(P->x, NUMWORDS, octet+1, KEYDIGITS*NN_DIGIT_LEN);
      //compute y
      NNModSqrOpt(alpha, P->x, param.p, param.omega, NUMWORDS);
      NNModMultOpt(alpha, alpha, P->x, param.p, param.omega, NUMWORDS);
      NNModMultOpt(tmp, param.E.a, P->x, param.p, param.omega, NUMWORDS);
      NNModAdd(tmp, tmp, alpha, param.p, NUMWORDS);
      NNModAdd(tmp, tmp, param.E.b, param.p, NUMWORDS);
      NNModSqrRootOpt(P->y, tmp, param.p, NUMWORDS, param.omega);
      if(octet[0] == 3){
	NNModSub(P->y, param.p, P->y, param.p, NUMWORDS);
      }
      return KEYDIGITS*NN_DIGIT_LEN+1;
    }
    return -1;
  }

  int ECC_check_point(Point *P){
    NN_DIGIT tmp1[NUMWORDS], tmp2[NUMWORDS];
    
    if (NNZero(P->x, NUMWORDS))
      return -1;
    if (NNCmp(P->x, param.p, NUMWORDS) >= 0)
      return -1;
    if (NNZero(P->y, NUMWORDS))
      return -1;
    if (NNCmp(P->y, param.p, NUMWORDS) >= 0)
      return -1;
    
    memset(tmp1, 0, NUMWORDS*NN_DIGIT_LEN);
    memset(tmp2, 0, NUMWORDS*NN_DIGIT_LEN);
    NNModSqrOpt(tmp2, P->x, param.p, param.omega, NUMWORDS);
    NNModMultOpt(tmp2, tmp2, P->x, param.p, param.omega, NUMWORDS);
    NNModMultOpt(tmp1, P->x, param.E.a, param.p, param.omega, NUMWORDS);
    NNModAdd(tmp2, tmp1, tmp2, param.p, NUMWORDS);
    NNModAdd(tmp2, tmp2, param.E.b, param.p, NUMWORDS);
    NNModSqrOpt(tmp1, P->y, param.p, param.omega, NUMWORDS);
    if(NNCmp(tmp1, tmp2, NUMWORDS) != 0)
      return -2;
    
    return 1;
  }

  bool ECC_point_is_zero(Point *P){
    return p_iszero(P);
  }
  
  // curve routines
  // P0 = P1 + P2
  void ECC_add(Point * P0, Point * P1, Point * P2)
  {
#ifdef AFFINE
    c_add_affine(P0, P1, P2);
#else
    c_add(P0, P1, P2);
#endif
  }

#ifdef AFFINE
  void ECC_add_affine(Point * P0, Point * P1, Point * P2){
    c_add_affine(P0, P1, P2);
  }

  void ECC_dbl_affine(Point *P0, Point *P1){
    c_dbl_affine(P0, P1);
  }
#endif

  // scalar point multiplication
  // P0 = n*P1
  // P0 and P1 can not be same point
  void ECC_mul(Point * P0, Point * P1, NN_DIGIT * n)
  {
#ifdef AFFINE
    int16_t i, tmp;

    // clear point
    p_clear(P0);
    tmp = NNBits(n, NUMWORDS);

    for(i = tmp-1; i >= 0; i--) {
		watchdog_periodic();
		c_dbl_affine(P0, P0);

      if (b_testbit(n, i)){      	
	c_add_affine(P0, P0, P1);
      }
    }

#else//projective
    int16_t i, tmp;
    NN_DIGIT Z0[NUMWORDS];
    NN_DIGIT Z1[NUMWORDS];

    // clear point
    p_clear(P0);
    
    //convert to Jprojective coordinate
    NNAssignZero(Z0, NUMWORDS);
    NNAssignZero(Z1, NUMWORDS);
    Z1[0] = 0x01;

    tmp = NNBits(n, NUMWORDS);

    for (i = tmp-1; i >= 0; i--)
    {
		watchdog_periodic();
		c_dbl_projective(P0, Z0, P0, Z0);

      if (b_testbit(n, i))
      {        	
#ifdef ADD_MIX
	c_add_mix(P0, Z0, P0, Z0, P1);
#else
	c_add_projective(P0, Z0, P0, Z0, P1, Z1);
#endif
      }
    }   
	//convert back to affine coordinate
    if (!Z_is_one(Z0))
    {
      NNModInv(Z1, Z0, param.p, NUMWORDS);
      NNModMultOpt(Z0, Z1, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->x, P0->x, Z0, param.p, param.omega, NUMWORDS);
      NNModMultOpt(Z0, Z0, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->y, P0->y, Z0, param.p, param.omega, NUMWORDS);
    }
#endif

  }

#ifdef SLIDING_WIN
  // precompute the array of base point for sliding window method
  void ECC_win_precompute(Point * baseP, Point * pointArray)
  {
    uint8_t i;
    
    NNAssign(pointArray[0].x, baseP->x, NUMWORDS);
    NNAssign(pointArray[0].y, baseP->y, NUMWORDS);

    for(i = 1; i < NUM_POINTS; i++) {
		watchdog_periodic();
#ifdef AFFINE
      c_add_affine(&(pointArray[i]), &(pointArray[i-1]), baseP);
#else
      c_add(&(pointArray[i]), &(pointArray[i-1]), baseP);
#endif
    }
    
	for (i = 0; i < NUM_MASKS; i++) {
      mask[i] = BASIC_MASK << (W_BITS*i);
	}
  }


  // scalar point multiplication
  // P0 = n*basepoint
  // pointArray is array of basepoint, pointArray[0] = basepoint, pointArray[1] = 2*basepoint ...
static void win_mul(Point * P0, NN_DIGIT * n, Point * pointArray)
  {
#ifdef AFFINE
    int16_t i, tmp;
    int8_t j, k;
    NN_DIGIT windex;

    p_clear(P0);    
    tmp = NNDigits(n, NUMWORDS);

    for (i = tmp - 1; i >= 0; i--){
		watchdog_periodic();
		for (j = NN_DIGIT_BITS/W_BITS - 1; j >= 0; j--){
	for (k = 0; k < W_BITS; k++){
	  c_dbl_affine(P0, P0);
	}
        windex = mask[j] & n[i];
        if (windex){
          windex = windex >> (j*W_BITS);
	  c_add_affine(P0, P0, &(pointArray[windex-1]));
	}
      }	
    }

#else //projective    
    int16_t i, tmp;
    int8_t j;
    NN_DIGIT windex;
    NN_DIGIT Z0[NUMWORDS];
    NN_DIGIT Z1[NUMWORDS];
#ifndef REPEAT_DOUBLE
    int8_t k;
#endif

    p_clear(P0);
    
    //convert to Jprojective coordinate
    NNAssignZero(Z0, NUMWORDS);
    NNAssignZero(Z1, NUMWORDS);
    Z1[0] = 0x01;	
    
    tmp = NNDigits(n, NUMWORDS);

    for (i = tmp - 1; i >= 0; i--){ 
		for (j = NN_DIGIT_BITS/W_BITS - 1; j >= 0; j--){
			watchdog_periodic();

#ifndef REPEAT_DOUBLE
	for (k = 0; k < W_BITS; k++){
	  c_dbl_projective(P0, Z0, P0, Z0);
	}
#else
	c_m_dbl_projective(P0, Z0, W_BITS);
#endif

        windex = mask[j] & n[i];

        if (windex){
          windex = windex >> (j*W_BITS);
#ifdef ADD_MIX 
	  c_add_mix(P0, Z0, P0, Z0, &(pointArray[windex-1]));
#else
	  c_add_projective(P0, Z0, P0, Z0, &(pointArray[windex-1]), Z1);
#endif
	}
      }	
    }

       
    //convert back to affine coordinate
    if (!Z_is_one(Z0))
    {
    
      NNModInv(Z1, Z0, param.p, NUMWORDS);
      NNModMultOpt(Z0, Z1, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->x, P0->x, Z0, param.p, param.omega, NUMWORDS);
      NNModMultOpt(Z0, Z0, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->y, P0->y, Z0, param.p, param.omega, NUMWORDS);
    }

#endif    
  }
  
  /**
   * P0 = n * point, point is pointArray[0]
   * win_precompute must be called before win_mul
   */
  void ECC_win_mul(Point * P0, NN_DIGIT * n, Point * pointArray)
  {
    win_mul(P0, n, pointArray);
  }
  
  /**
   * P0 = n * basepoint of curve
   * Don't need to call win_precompute before this func, cause init() has called win_precompute
   */
  void ECC_win_mul_base(Point * P0, NN_DIGIT * n)
  {
    win_mul(P0, n, pBaseArray);
  }

#ifdef PROJECTIVE

  void ECC_win_precompute_Z(Point * baseP, Point * pointArray, ZCoordinate *ZList){
    uint8_t i;
    
    NNAssign(pointArray[0].x, baseP->x, NUMWORDS);
    NNAssign(pointArray[0].y, baseP->y, NUMWORDS);
    NNAssignZero(ZList[0].z, NUMWORDS);
    ZList[0].z[0] = 0x01;

    for (i = 1; i < NUM_POINTS; i++){
		watchdog_periodic();
#ifdef ADD_MIX      
      c_add_mix(&(pointArray[i]), ZList[i].z, &(pointArray[i-1]), ZList[i-1].z, baseP);
#else
      c_add_projective(&(pointArray[i]), ZList[i].z, &(pointArray[i-1]), ZList[i-1].z, baseP, ZList[0].z);
#endif
    }

  }

  void ECC_win_mul_Z(Point *P0, NN_DIGIT *n, Point *pointArray, ZCoordinate *ZList){
    int16_t i, tmp;
    int8_t j;
    NN_DIGIT windex;
    NN_DIGIT Z0[NUMWORDS];
    NN_DIGIT Z1[NUMWORDS];
#ifndef REPEAT_DOUBLE
    int8_t k;
#endif

    p_clear(P0);
    
    //convert to Jprojective coordinate
    NNAssignZero(Z0, NUMWORDS);
    NNAssignZero(Z1, NUMWORDS);

    tmp = NNDigits(n, NUMWORDS);

    for (i = tmp - 1; i >= 0; i--){ 
      for (j = NN_DIGIT_BITS/W_BITS - 1; j >= 0; j--){
		  watchdog_periodic();
		  
#ifndef REPEAT_DOUBLE
	for (k = 0; k < W_BITS; k++){
	  c_dbl_projective(P0, Z0, P0, Z0);
	}
#else
	c_m_dbl_projective(P0, Z0, W_BITS);
#endif

        windex = mask[j] & n[i];

        if (windex){
          windex = windex >> (j*W_BITS);
	  c_add_projective(P0, Z0, P0, Z0, &(pointArray[windex-1]), ZList[windex-1].z);
	}
      }	
    }

       
    //convert back to affine coordinate
    if (!Z_is_one(Z0))
    {
    
      NNModInv(Z1, Z0, param.p, NUMWORDS);
      NNModMultOpt(Z0, Z1, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->x, P0->x, Z0, param.p, param.omega, NUMWORDS);
      NNModMultOpt(Z0, Z0, Z1, param.p, param.omega, NUMWORDS);
      NNModMultOpt(P0->y, P0->y, Z0, param.p, param.omega, NUMWORDS);
    }    
  }
#endif  //PROJECTIVE

#endif  //SLIDING_WIN

  Params * ECC_get_param(){
    return &param;
  }

#ifndef AFFINE  
  void ECC_add_proj(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1, Point * P2, NN_DIGIT * Z2){
    return c_add_projective(P0, Z0, P1, Z1, P2, Z2);
  }

  void ECC_dbl_proj(Point * P0, NN_DIGIT *Z0, Point * P1, NN_DIGIT * Z1){
    return c_dbl_projective(P0, Z0, P1, Z1);
  }
#endif

  void ECC_gen_private_key(NN_DIGIT *PrivateKey){

    NN_UINT order_digit_len, order_bit_len;
    bool done = FALSE;
    uint8_t ri;
    NN_DIGIT digit_mask;

    order_bit_len = NNBits(param.r, NUMWORDS);
    order_digit_len = NNDigits(param.r, NUMWORDS);

    while(!done){

      for (ri=0; ri<order_digit_len; ri++){
#ifdef THIRTYTWO_BIT_PROCESSOR
	PrivateKey[ri] = ((uint32_t)rand() << 16)^((uint32_t)rand());
#else
	PrivateKey[ri] = (NN_DIGIT)rand();
#endif
      }

      for (ri=order_digit_len; ri<NUMWORDS; ri++){
	PrivateKey[ri] = 0;
      }

      if (order_bit_len % NN_DIGIT_BITS != 0){
	digit_mask = MAX_NN_DIGIT >> (NN_DIGIT_BITS - order_bit_len % NN_DIGIT_BITS);
	PrivateKey[order_digit_len - 1] = PrivateKey[order_digit_len - 1] & digit_mask;
      }
      NNModSmall(PrivateKey, param.r, NUMWORDS);

      if (NNZero(PrivateKey, NUMWORDS) != 1)
	done = TRUE;
    }

  }

void ECC_gen_public_key(Point *PublicKey, NN_DIGIT *PrivateKey){

#ifdef SLIDING_WIN
    win_mul(PublicKey, PrivateKey, pBaseArray);    
#else
    ECC_mul(PublicKey, &(param.G), PrivateKey);
#endif
  }

/* --- Extension TUD -------------------------------------------------------- */

/*
 * @brief Assign Points P0 = P1.
 */
void ECC_assign(Point *P0, Point *P1) {
	NNAssign(P0->x, P1->x, NUMWORDS);
	NNAssign(P0->y, P1->y, NUMWORDS);
}

// Legendre-Symbol L(a/r) == 1? is a quadratic residue?
/*bool ECC_is_sqr(NN_DIGIT * a, NN_UINT digits) {
	NN_DIGIT z[NUMWORDS];
	NN_DIGIT u[NUMWORDS];
	int res;
	
	if (NNZero(a, NUMWORDS)) return 1;
	
	NNAssignOne(u, NUMWORDS);
	NNModSub(z, param.r, u, param.p, NUMWORDS);
	NNAssignDigit(u, 2, NUMWORDS);
	NNModDivOpt(z, z, u, param.p, NUMWORDS);
	NNModExp(u, a, z, NUMWORDS, param.p, NUMWORDS);
	res = NNOne(u, NUMWORDS) == TRUE;
		
	return res;
}*/

/*
 * Find matching y to given x and return Point. P = (x, y) e E
 * x has to be x mod p
 * @returns if valid point
 */
/*void ECC_compY(Point * P) {
    NN_DIGIT tmp[NUMWORDS];
	NN_DIGIT x[NUMWORDS];
	
	NNAssign(x, h, NUMWORDS);
	// Compute y
	for(;;) {
		NNModRandom(x, param.r, NUMWORDS);
		NNModSqrOpt(tmp, x, param.p, param.omega, NUMWORDS);
		NNModAdd(tmp, tmp, param.E.a, param.p, NUMWORDS);
		NNModMultOpt(tmp, tmp, P->x, param.p, param.omega, NUMWORDS);
		NNModAdd(tmp, tmp, param.E.b, param.p, NUMWORDS);	
printf("1\n");
//		if (NNEven(param.r, NUMWORDS)) {
			if (ECC_is_sqr(tmp, NUMWORDS)) {
				break;
			}
//		}
		NNModSqrOpt(P->x, P->x, param.p, param.omega, NUMWORDS);
		NNAssignOne(tmp, NUMWORDS);
		NNModAdd(P->x, P->x, tmp, param.p, NUMWORDS);
printf("2\n");
	}
	NNModSqrRootOpt(P->y, tmp, param.p, NUMWORDS, NULL);	
printf("3\n");

	// Set x
	NNAssign(P->x, x, NUMWORDS);
	
//	if (element_sgn(p->y) < 0) element_neg(p->y, p->y);
//	
#ifdef TPSSC
	TPParams tp;
	get_TP_param(&tp);
	ECC_mul(P, P, tp.c);
printf("4\n");
#endif
}*/

/*
 * @brief Find random point P based on generator G.
 */
//void ECC_Random_solvefory(Point * P) {
//	NN_DIGIT x[NUMWORDS]; 
//	
//	NNModRandom(x, param.r, NUMWORDS);
//	ECC_compY(P, x);
//}

/*
 * @brief Find point P based on generator G and hash. (~10sec per call)
 */
void ECC_Random_Hash(Point * P, NN_DIGIT * h) {
	NN_DIGIT x[NUMWORDS]; 
	NN_DIGIT tmp[NUMWORDS]; 
	
//	NNMod(x, h, NUMWORDS, param.r, NUMWORDS);
	NNDiv(NULL, tmp, h, 2*NUMWORDS, param.r, NNDigits(param.r, NUMWORDS));
	NNAssignZero(x, NUMWORDS);
	NNAssign(x, tmp, NNDigits(param.r, NUMWORDS));

	ECC_mul(P, &(param.G), x);
}

/*
 * @brief Find random point P based on generator G.(~10sec per call)
 */
void ECC_Random_PointMul(Point * P) {
	NN_DIGIT x[NUMWORDS]; 
	
	NNModRandom(x, param.r, NUMWORDS);
	ECC_mul(P, &(param.G), x);
}


