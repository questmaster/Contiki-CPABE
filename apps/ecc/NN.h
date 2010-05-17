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
 * $Id: NN.h,v 1.9 2007/09/12 18:17:06 aliu3 Exp $
 */

#ifndef _NN_H_
#define _NN_H_

#include <stdlib.h>

// boolean datatype
typedef int8_t bool;
#define TRUE 1
#define FALSE 0

/*
// frequency (in MHz) of the imote2 processor
#if CORE_FREQ == 13
#define CORE_BUS 13
#elif CORE_FREQ == 104
#define CORE_BUS 104
#elif CORE_FREQ == 208
#define CORE_BUS 208
#elif CORE_FREQ == 416
#define CORE_BUS 208
#endif
*/

//#define BARRETT_REDUCTION
//#define HYBRID_MULT  //hybrid multiplication
//#define HYBRID_SQR //hybrd squaring
//#define CURVE_OPT  //optimization for SECG curves


#define MODINVOPT

#ifdef TEST_VECTOR

#define KEY_BIT_LEN 160
#define HYBRID_MUL_WIDTH5

#else

#if defined (SECP128R1) || defined (SECP128R2)
#define KEY_BIT_LEN 128
#define HYBRID_MUL_WIDTH4
#else
#if defined (SECP160K1) || defined (SECP160R1) || defined (SECP160R2)
#define KEY_BIT_LEN 160
#define HYBRID_MUL_WIDTH5  //column width=5 for hybrid multiplication
#else
#if defined (SECP192K1) || defined (SECP192R1) || defined (SS192K2) || defined (SS192K2S)
#define KEY_BIT_LEN 192
#define HYBRID_MUL_WIDTH4
#else
#if defined (SS512K2) || defined (SS512K2S)
//#define BARRETT_REDUCTION
#define KEY_BIT_LEN 512
#define HYBRID_MUL_WIDTH5
#endif // end of 512
#endif  //end of 192
#endif  //end of 160
#endif  //end of 128

#endif  //TEST_VECTOR



//mica, mica2, micaz
#ifdef CONTIKI_TARGET_MICAZ
#define EIGHT_BIT_PROCESSOR
#define INLINE_ASM
#endif

//telosb
#ifdef CONTIKI_TARGET_SKY
#define SIXTEEN_BIT_PROCESSOR
#define INLINE_ASM
#endif

//imote2
#ifdef CONTIKI_TARGET_IMOTE2
#define THIRTYTWO_BIT_PROCESSOR
#define INLINE_ASM
#endif




#ifdef EIGHT_BIT_PROCESSOR

/* Type definitions */
typedef uint8_t NN_DIGIT;
typedef uint16_t NN_DOUBLE_DIGIT;

/* Types for length */
typedef uint8_t NN_UINT;
typedef uint16_t NN_UINT2;

/* Length of digit in bits */
#define NN_DIGIT_BITS 8

/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS/8)

/* Maximum value of digit */
#define MAX_NN_DIGIT 0xff

/* Number of digits in key
 * used by optimized mod multiplication (ModMultOpt) and optimized mod square (ModSqrOpt)
 *
 */
#define KEYDIGITS (KEY_BIT_LEN/NN_DIGIT_BITS) //20

/* Maximum length in digits */
#define MAX_NN_DIGITS (KEYDIGITS+1)

/* buffer size
 *should be large enough to hold order of base point
 */
#define NUMWORDS MAX_NN_DIGITS

#endif  //EIGHT_BIT_PROCESSOR



#ifdef SIXTEEN_BIT_PROCESSOR

/* Type definitions */
typedef uint16_t NN_DIGIT;
typedef uint32_t NN_DOUBLE_DIGIT;

/* Types for length */
typedef uint8_t NN_UINT;
typedef uint16_t NN_UINT2;

/* Length of digit in bits */
#define NN_DIGIT_BITS 16

/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS/8)

/* Maximum value of digit */
#define MAX_NN_DIGIT 0xffff

/* Number of digits in key
 * used by optimized mod multiplication (ModMultOpt) and optimized mod square (ModSqrOpt)
 *
 */
#define KEYDIGITS (KEY_BIT_LEN/NN_DIGIT_BITS) //10

/* Maximum length in digits */
#define MAX_NN_DIGITS (KEYDIGITS+1)

/* buffer size
 *should be large enough to hold order of base point
 */
#define NUMWORDS MAX_NN_DIGITS

#endif  //SIXTEEN_BIT_PROCESSOR



#ifdef THIRTYTWO_BIT_PROCESSOR

/* Type definitions */
typedef uint32_t NN_DIGIT;
typedef uint64_t NN_DOUBLE_DIGIT;

/* Types for length */
typedef uint8_t NN_UINT;
typedef uint16_t NN_UINT2;

/* Length of digit in bits */
#define NN_DIGIT_BITS 32

/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS/8)

/* Maximum value of digit */
#define MAX_NN_DIGIT 0xffffffff

/* Number of digits in key
 * used by optimized mod multiplication (ModMultOpt) and optimized mod square (ModSqrOpt)
 *
 */
#define KEYDIGITS (KEY_BIT_LEN/NN_DIGIT_BITS) //5

/* Maximum length in digits */
#define MAX_NN_DIGITS (KEYDIGITS+1)

/* buffer size
 *should be large enough to hold order of base point
 */
#define NUMWORDS MAX_NN_DIGITS

#endif  //THIRTYTWO_BIT_PROCESSOR



struct Barrett{
  NN_DIGIT mu[2*MAX_NN_DIGITS+1];
  NN_UINT mu_len;
  NN_UINT km;  //length of moduli m, m_{k-1} != 0
};
typedef struct Barrett Barrett;

/*
 * Prototypes of functions
 *
 */
/*
 CONVERSIONS
 Decode (a, digits, b, len)   Decodes character string b into a.
 Encode (a, len, b, digits)   Encodes a into character string b.
 
 ASSIGNMENTS
 Assign (a, b, digits)        Assigns a = b.
 ASSIGN_DIGIT (a, b, digits)  Assigns a = b, where b is a digit.
 AssignZero (a, digits)    Assigns a = 0.
 Assign2Exp (a, b, digits)    Assigns a = 2^b.
 
 ARITHMETIC OPERATIONS
 Add (a, b, c, digits)        Computes a = b + c.
 Sub (a, b, c, digits)        Computes a = b - c.
 Mult (a, b, c, digits)       Computes a = b * c.
 LShift (a, b, c, digits)     Computes a = b * 2^c.
 RShift (a, b, c, digits)     Computes a = b / 2^c.
 Div (a, b, c, cDigits, d, dDigits)  Computes a = c div d and b = c mod d.
 
 NUMBER THEORY
 Mod (a, b, bDigits, c, cDigits)  Computes a = b mod c.
 ModMult (a, b, c, d, digits) Computes a = b * c mod d.
 ModExp (a, b, c, cDigits, d, dDigits)  Computes a = b^c mod d.
 ModInv (a, b, c, digits)     Computes a = 1/b mod c.
 ModNeg(a, b, c, digits)      Computes a = -b mod c.
 ModDiv(a, b, c, digits)      Computes a = (b / c) mod d.
 ModDivOpt(a, b, c, digits)      Computes a = (b / c) mod d using the opt way
 Gcd (a, b, c, digits)        Computes a = gcd (b, c).
 
 OTHER OPERATIONS
 EVEN (a, digits)             Returns 1 iff a is even.
 Cmp (a, b, digits)           Returns sign of a - b.
 EQUAL (a, digits)            Returns 1 iff a = b.
 Zero (a, digits)             Returns 1 iff a = 0.
 Digits (a, digits)           Returns significant length of a in digits.
 Bits (a, digits)             Returns significant length of a in bits.
 LucExp(a, b, k, inv2, p, digits)	 Computes lucas(2*b,k)/2.
 */

/* CONVERSIONS */

//Decodes character string b into a.
extern void NNDecode(NN_DIGIT * a, NN_UINT digits, unsigned char * b, NN_UINT len);
//Encodes a into character string b.
#ifdef CODE_SIZE
extern void NNEncode(unsigned char * a, NN_UINT digits, NN_DIGIT * b, NN_UINT len) __attribute__ ((noinline));
#else
extern void NNEncode(unsigned char * a, NN_UINT digits, NN_DIGIT * b, NN_UINT len);
#endif

/* ASSIGNMENT */
//Assigns a = b.
extern void NNAssign(NN_DIGIT * a, NN_DIGIT * b, NN_UINT digits);
//Assigns a = b, where b is a digit.
extern void NNAssignDigit(NN_DIGIT * a, NN_DIGIT b, NN_UINT digits);
//Assigns a = 0.
extern void NNAssignZero(NN_DIGIT * a, NN_UINT digits);
//Assigns a = 2^b.
extern void NNAssign2Exp(NN_DIGIT * a, NN_UINT2 b, NN_UINT digits);

/* ARITHMETIC OPERATIONS */
//Computes a = b + c.
extern NN_DIGIT NNAdd(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits);
//Computes a = b - c.
extern NN_DIGIT NNSub(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits);
//Computes a = b * c.
extern void NNMult(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits);
//Computes a = c div d and b = c mod d.
extern void NNDiv(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT cDigits, NN_DIGIT * d, NN_UINT dDigits);
//Computes a = b * 2^c.
extern NN_DIGIT NNLShift(NN_DIGIT * a, NN_DIGIT * b, NN_UINT c, NN_UINT digits);
//Computes a = b / 2^c.
extern NN_DIGIT NNRShift(NN_DIGIT * a, NN_DIGIT * b, NN_UINT c, NN_UINT digits);
//Computes a = b + c*d, where c is a digit
extern NN_DIGIT NNAddDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, NN_UINT digits);

/* NUMBER THEORY */
//Computes a = b mod c.
extern void NNMod(NN_DIGIT * a, NN_DIGIT * b, NN_UINT bDigits, NN_DIGIT * c, NN_UINT cDigits);

extern void NNModBarrettInit(NN_DIGIT *c, NN_UINT cDigits, Barrett *pbuf);

extern void NNBarrettSetBuf(Barrett *pbuf);

extern void NNModBarrett(NN_DIGIT *a, NN_DIGIT *b, NN_UINT bDigits, NN_DIGIT *c, NN_UINT cDigits);
//Computes b = b mod c, suppose the b is just one bit longer than c
extern void NNModSmall(NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits);
//Computes a = (b + c) mod d
extern void NNModAdd(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits);
//Computes a = (b - c) mod d
extern void NNModSub(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits);
//Computes a = -b mod c
extern void NNModNeg(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits); 
//Computes a = (b * c) mod d.
extern void NNModMult(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits);
//Computes a = (b / c) mod d.
extern void NNModDiv(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits);
//Computes a = (b * c) mod d, for d is generalized mersenne number, d = 2^m - omega
extern void NNModMultOpt(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_DIGIT * omega, NN_UINT digits);
//Computes a = (b / c) mod d using the opt way
extern void NNModDivOpt(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits);
//Computes a = b^2 mod d
extern void NNModSqr(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * d, NN_UINT digits);
//Computes a = b^2 mod d
extern void NNModSqrOpt(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * d, NN_DIGIT * omega, NN_UINT digits);
//Computes a = b^c mod d.
extern void NNModExp(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT cDigits, NN_DIGIT * d, NN_UINT dDigits);
//Computes a = 1/b mod c.
extern void NNModInv(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits);
//Computes a = gcd (b, c).
extern void NNGcd(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits);

extern int NNModSqrRootOpt(NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits, NN_DIGIT *omega);

//extern void ModDivOpt(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, );

/* OTHER OPERATIONS */
//Returns sign of a - b.
extern int NNCmp(NN_DIGIT * a, NN_DIGIT * b, NN_UINT digits);
//Returns 1 iff a = 0.
extern int NNZero(NN_DIGIT * a, NN_UINT digits);
//Returns 1 iff a = 1.
extern int NNOne(NN_DIGIT * a, NN_UINT digits);
//Returns significant length of a in bits.
extern unsigned int NNBits(NN_DIGIT * a, NN_UINT digits);
// test whether the ith bit in a is one
extern NN_DIGIT NNTestBit(NN_DIGIT * a, int16_t i);
//Returns significant length of a in digits.
extern unsigned int NNDigits(NN_DIGIT * a, NN_UINT digits);
//Returns 1 iff a = b.
extern bool NNEqual(NN_DIGIT * a, NN_DIGIT * b, NN_UINT digits);
//Returns 1 iff a is even.
extern bool NNEven(NN_DIGIT * a, NN_UINT digits);
//Returns the lucas exponentatiation for the Tate Pairing lucas(2*b,k)/2
extern bool NNLucExp(NN_DIGIT * a,NN_DIGIT * b, NN_DIGIT * k,NN_DIGIT * inv2,NN_DIGIT * p,NN_UINT digits);



#endif
