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
 * NN2.h
 * Provide the functions of big Natural Complex Numbers.
 *
 * Author: Panos Kampanakis
 * Date: 02/04/2007
 */

#ifndef _NN2_H_
#define _NN2_H_

#include "NN.h"

// Complex numbers for Tate Pairing of distorted mapped points
struct nn2_num
{
   	// curve's coefficients
   	NN_DIGIT r[NUMWORDS];
   	NN_DIGIT i[NUMWORDS];
};
typedef struct nn2_num NN2_NUMBER;

//multiplies two complex numbers
extern void NN2ModMult(NN2_NUMBER * a, NN2_NUMBER * b, NN2_NUMBER * c, NN_DIGIT * d, NN_UINT digits);
//square of a complex number
extern void NN2ModSqr(NN2_NUMBER * a, NN2_NUMBER * b, NN_DIGIT * d, NN_UINT digits);
//computes a = b^c mod d
extern void NN2ModExp(NN2_NUMBER * a, NN2_NUMBER * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits);
//assign a = b^-1 mod c
extern void NN2ModInv(NN2_NUMBER * a,NN2_NUMBER * b,NN_DIGIT * c,NN_UINT digits);
//assign a = b
extern void NN2Assign(NN2_NUMBER * a,NN2_NUMBER * b,NN_UINT digits);
//assign a = (b+i0)
extern void NN2AssignNN(NN2_NUMBER * a,NN_DIGIT * b,NN_UINT digits);
//Return the lucas exponentatiation for the Tate Pairing lucas(2*b,k)/2
extern bool NN2LucExp(NN2_NUMBER * a,NN2_NUMBER * b, NN_DIGIT * k,NN_DIGIT * inv2,NN_DIGIT * d,NN_UINT digits);
//Generates Random Complex number modulo b
extern void NN2ModRandom(NN2_NUMBER * a, NN_DIGIT * b, NN_UINT digits);

#endif
