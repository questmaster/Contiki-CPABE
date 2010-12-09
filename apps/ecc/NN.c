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
 * $Id: NNM.nc,v 1.8 2007/11/01 23:02:59 aliu3 Exp $
 * module NNM, provide interface NN
 * modified from nn.h and nn.c from RSAREF 2.0
 */

#include <watchdog.h>
#include <NN.h>
#include <string.h>
#include <CurveParam.h>


#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define DIGIT_MSB(x) (NN_DIGIT)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) (NN_DIGIT)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZero (a, digits); a[0] = b;}
#define NN_EQUAL(a, b, digits) (! NN_Cmp (a, b, digits))
#define NN_EVEN(a, digits) (((digits) == 0) || ! (a[0] & 1))


/*
   CONVERSIONS
   NN_Decode (a, digits, b, len)   Decodes character string b into a.
   NN_Encode (a, len, b, digits)   Encodes a into character string b.

   ASSIGNMENTS
   NN_Assign (a, b, digits)        Assigns a = b.
   NN_ASSIGN_DIGIT (a, b, digits)  Assigns a = b, where b is a digit.
   NN_AssignZero (a, digits)    Assigns a = 0.
   NN_Assign2Exp (a, b, digits)    Assigns a = 2^b.
     
   ARITHMETIC OPERATIONS
   NN_Add (a, b, c, digits)        Computes a = b + c.
   NN_Sub (a, b, c, digits)        Computes a = b - c.
   NN_Mult (a, b, c, digits)       Computes a = b * c.
   NN_LShift (a, b, c, digits)     Computes a = b * 2^c.
   NN_RShift (a, b, c, digits)     Computes a = b / 2^c.
   NN_Div (a, b, c, cDigits, d, dDigits)  Computes a = c div d and b = c mod d.

   NUMBER THEORY
   NN_Mod (a, b, bDigits, c, cDigits)  Computes a = b mod c.
   NN_ModMult (a, b, c, d, digits) Computes a = b * c mod d.
   NN_ModExp (a, b, c, cDigits, d, dDigits)  Computes a = b^c mod d.
   NN_ModInv (a, b, c, digits)     Computes a = 1/b mod c.
   NN_Gcd (a, b, c, digits)        Computes a = gcd (b, c).

   OTHER OPERATIONS
   NN_EVEN (a, digits)             Returns 1 iff a is even.
   NN_Cmp (a, b, digits)           Returns sign of a - b.
   NN_EQUAL (a, digits)            Returns 1 iff a = b.
   NN_Zero (a, digits)             Returns 1 iff a = 0.
   NN_Digits (a, digits)           Returns significant length of a in digits.
   NN_Bits (a, digits)             Returns significant length of a in bits.
*/

               void  NN_DigitDiv (NN_DIGIT *a, NN_DIGIT b[2], NN_DIGIT c);
               void  NN_Decode (NN_DIGIT *a, NN_UINT digits, unsigned char *b, NN_UINT len);
               void  NN_Encode (unsigned char *a, NN_UINT len, NN_DIGIT *b, NN_UINT digits);
               void  NN_Assign (NN_DIGIT *a, NN_DIGIT *b, NN_UINT digits);
               void  NN_AssignZero (NN_DIGIT *a, NN_UINT digits);
               void  NN_Assign2Exp (NN_DIGIT *a, NN_UINT2 b, NN_UINT digits);
#ifdef CODE_SIZE
		   NN_DIGIT  NN_Add (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits) __attribute__ ((noinline));
		   NN_DIGIT  NN_Sub (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits) __attribute__ ((noinline));
			   void  NN_Mult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits) __attribute__ ((noinline));
			   void  NN_Sqr(NN_DIGIT *a, NN_DIGIT *b, NN_UINT digits) __attribute__ ((noinline));
#else
		   NN_DIGIT  NN_Add (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits);
		   NN_DIGIT  NN_Sub (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits);
		   void  NN_Mult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits);
		   void  NN_Sqr(NN_DIGIT *a, NN_DIGIT *b, NN_UINT digits);
#endif
		   NN_DIGIT  NN_LShift (NN_DIGIT *a, NN_DIGIT *b, NN_UINT c, NN_UINT digits);
           NN_DIGIT  NN_RShift (NN_DIGIT *a, NN_DIGIT *b, NN_UINT c, NN_UINT digits);
               void  NN_Div (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT cDigits, NN_DIGIT *d, NN_UINT dDigits);
               void  NN_Mod (NN_DIGIT *a, NN_DIGIT *b, NN_UINT bDigits, NN_DIGIT *c, NN_UINT cDigits);
               void  NN_ModMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d, NN_UINT digits);
               void  NN_ModExp (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT cDigits, NN_DIGIT *d, NN_UINT dDigits);
               void  NN_ModInv (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits);
               void  NN_Gcd (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits);
                int  NN_Cmp (NN_DIGIT *a, NN_DIGIT *b, NN_UINT digits);
                int  NN_Zero (NN_DIGIT *a, NN_UINT digits);
       unsigned int  NN_Bits (NN_DIGIT *a, NN_UINT digits);
       unsigned int  NN_Digits (NN_DIGIT *a, NN_UINT digits);
    static NN_DIGIT  NN_AddDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, NN_UINT digits);
    static NN_DIGIT  NN_SubDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, NN_UINT digits);
static unsigned int  NN_DigitBits (NN_DIGIT a);
	       void  NN_Lucas (NN_DIGIT * a,NN_DIGIT * b, NN_DIGIT * k,NN_DIGIT * p,NN_UINT digits);

  Barrett * pBarrett;

/* Return b * c, where b and c are NN_DIGITs. The result is a NN_DOUBLE_DIGIT
  */
#ifdef INLINE_ASM

#ifdef CONTIKI_TARGET_MICAZ
#define NN_DigitMult(b, c) (NN_DOUBLE_DIGIT)(b) * (c)
#endif

#ifdef CONTIKI_TARGET_SKY
     
NN_DOUBLE_DIGIT NN_DigitMult(NN_DIGIT b, NN_DIGIT c)
  {
    NN_DIGIT result_h;
    NN_DIGIT result_l;
            
    asm volatile ("mov %2, &0x130 \n\t"
                  "mov %3, &0x138 \n\t"
                  "mov &0x13C, %0 \n\t"
                  "mov &0x13A, %1 \n\t"
                  :"=r"(result_h), "=r"(result_l)
                  :"r"(b), "r"(c)
                  );

    return (((NN_DOUBLE_DIGIT)result_h << 16) ^ (NN_DOUBLE_DIGIT)result_l);
  }
  
#endif

#ifdef CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki
#define NN_DigitMult(b, c) (NN_DOUBLE_DIGIT)(b) * (c)
#endif

#else  //no inline assembly

#define NN_DigitMult(b, c) (NN_DOUBLE_DIGIT)(b) * (c)

#endif

  // test whether the ith bit in a is one
static  NN_DIGIT b_testbit(NN_DIGIT * a, int16_t i)
  {
      return (*(a + (i / NN_DIGIT_BITS)) & ((NN_DIGIT)1 << (i % NN_DIGIT_BITS)));
  }
  
  /* Sets a = b / c, where a and c are digits.

     Lengths: b[2].
     Assumes b[1] < c and HIGH_HALF (c) > 0. For efficiency, c should be
     normalized.
     from digit.c
  */
  void NN_DigitDiv (NN_DIGIT *a, NN_DIGIT b[2], NN_DIGIT c)
  {
    NN_DOUBLE_DIGIT t;
  
    t = (((NN_DOUBLE_DIGIT)b[1]) << NN_DIGIT_BITS) ^ ((NN_DOUBLE_DIGIT)b[0]);
  
    *a = t/c;
  }

  /* Decodes character string b into a, where character string is ordered
     from most to least significant.

    Lengths: a[digits], b[len].
     Assumes b[i] = 0 for i < len - digits * NN_DIGIT_LEN. (Otherwise most
     significant bytes are truncated.)
  */
  void NN_Decode (NN_DIGIT *a, NN_UINT digits, unsigned char *b, NN_UINT len)
  {
    NN_DIGIT t;
    int j;
    unsigned int i, u;
  
    for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
      t = 0;
      for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
        t |= ((NN_DIGIT)b[j]) << u;
      a[i] = t;
    }
  
    for (; i < digits; i++)
      a[i] = 0;
  }

  /* Encodes b into character string a, where character string is ordered
     from most to least significant.

     Lengths: a[len], b[digits].
     Assumes NN_Bits (b, digits) <= 8 * len. (Otherwise most significant
     digits are truncated.)
   */
  void NN_Encode (unsigned char *a, NN_UINT len, NN_DIGIT *b, NN_UINT digits)
  {
    NN_DIGIT t;
    int j;
    unsigned int i, u;

    for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
      t = b[i];
      for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
        a[j] = (unsigned char)(t >> u);
    }

    for (; j >= 0; j--)
      a[j] = 0;
  }

  /* Assigns a = b.

     Lengths: a[digits], b[digits].
   */
  void NN_Assign (NN_DIGIT *a, NN_DIGIT *b, NN_UINT digits)
  {
    memcpy(a, b, digits*NN_DIGIT_LEN);
  }

  /* Assigns a = 0.

     Lengths: a[digits].
   */
  void NN_AssignZero (NN_DIGIT *a, NN_UINT digits)
  {

    uint8_t i;

    for (i = 0; i < digits; i++)
      a[i] = 0;

  }

  /* Assigns a = 2^b.

     Lengths: a[digits].
     Requires b < digits * NN_DIGIT_BITS.
   */
  void NN_Assign2Exp (NN_DIGIT *a, NN_UINT2 b, NN_UINT digits)
  {
    NN_AssignZero (a, digits);

    if (b >= digits * NN_DIGIT_BITS)
      return;

    a[b / NN_DIGIT_BITS] = (NN_DIGIT)1 << (b % NN_DIGIT_BITS);
  }


  /* Computes a = b + c. Returns carry.
     a, b ,c can be same
     Lengths: a[digits], b[digits], c[digits].
   */
  NN_DIGIT NN_Add (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits)
  {
    //inline assembly for micaz
#ifdef INLINE_ASM
#ifdef CONTIKI_TARGET_MICAZ

    NN_DIGIT carry;
    if (digits == 0)
      return 0;

    asm volatile ("dec %4 \n\t"
		  "ld r0, Y+ \n\t"
		  "ld r2, Z+ \n\t"
		  "add r0, r2 \n\t"
		  "st X+, r0 \n\t"
		  "ADD_LOOP1: tst %4 \n\t"
		  "breq ADD_LOOP1_EXIT \n\t"
		  "dec %4 \n\t"
		  "ld r0, Y+ \n\t"
		  "ld r2, Z+ \n\t"
		  "adc r0, r2 \n\t"
		  "st X+, r0 \n\t"
		  "jmp ADD_LOOP1 \n\t"
		  "ADD_LOOP1_EXIT: clr %0 \n\t"
		  "rol %0 \n\t"
		  :"=a"(carry)
		  :"x"(a),"y"(b),"z"(c), "r"(digits)
		  :"r0", "r2"
		  );

    return carry;
    
#endif
#ifdef CONTIKI_TARGET_SKY
    NN_DIGIT carry;
    NN_UINT i;

    carry = 0;

    for (i = 0; i < digits; i++) {
      asm volatile ("rrc %1 \n\t"
                    "addc %3, %2 \n\t"
                    "mov %2, %0 \n\t"
                    "rlc %1 \n\t"
		    :"=r"(a[i]), "+r"(carry)
                    :"r"(b[i]), "r"(c[i])
                    );
    }
    return carry;
#endif

#ifdef CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki
    NN_DIGIT carry, ai;
    NN_UINT i;

    carry = 0;

    for (i = 0; i < digits; i++) {
      if ((ai = b[i] + carry) < carry)
        ai = c[i];
      else if ((ai += c[i]) < c[i])
        carry = 1;
      else
        carry = 0;
      a[i] = ai;
    }
    return (carry);
#endif

#else  //no inline assembly

    NN_DIGIT carry, ai;
    NN_UINT i;

    carry = 0;

    for (i = 0; i < digits; i++) {
      if ((ai = b[i] + carry) < carry)
        ai = c[i];
      else if ((ai += c[i]) < c[i])
        carry = 1;
      else
        carry = 0;
      a[i] = ai;
    }
    return carry;
#endif

  }

  /* Computes a = b - c. Returns borrow.
     a, b, c can be same
     Lengths: a[digits], b[digits], c[digits].
   */
  NN_DIGIT NN_Sub (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits) 
  {
	  watchdog_periodic();
#ifdef INLINE_ASM
#ifdef CONTIKI_TARGET_MICAZ

    NN_DIGIT borrow;
    if (digits == 0)
      return 0;

    asm volatile ("dec %4 \n\t"
		  "ld r0, Y+ \n\t"
		  "ld r2, Z+ \n\t"
		  "sub r0, r2 \n\t"
		  "st X+, r0 \n\t"
		  "SUB_LOOP1: tst %4 \n\t"
		  "breq SUB_LOOP1_EXIT \n\t"
		  "dec %4 \n\t"
		  "ld r0, Y+ \n\t"
		  "ld r2, Z+ \n\t"
		  "sbc r0, r2 \n\t"
		  "st X+, r0 \n\t"
		  "jmp SUB_LOOP1 \n\t"
		  "SUB_LOOP1_EXIT: clr %0 \n\t"
		  "rol %0 \n\t"
		  :"=a"(borrow)
		  :"x"(a),"y"(b),"z"(c), "r"(digits)
		  :"r0", "r2"
		  );

    return borrow;

#endif
#ifdef CONTIKI_TARGET_SKY
    NN_DIGIT borrow;
    NN_UINT i;

    borrow = 1;

    for (i = 0; i < digits; i++) {
      asm volatile ("rrc %1 \n\t"
                    "subc %3, %2 \n\t"
                    "mov %2, %0 \n\t"
                    "rlc %1 \n\t"
                    :"=r"(a[i]), "+r"(borrow)
                    :"r"(b[i]), "r"(c[i])
                    );
    }
    borrow = borrow ^ 0x0001;
    return borrow;
#endif

#ifdef CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki
    NN_DIGIT ai, borrow;
    NN_UINT i;

    borrow = 0;

    for (i = 0; i < digits; i++) {
      if ((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
        ai = MAX_NN_DIGIT - c[i];
      else if ((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
        borrow = 1;
      else
        borrow = 0;
      a[i] = ai;
    }
    return (borrow);
#endif

#else  //no inline assembly

    NN_DIGIT ai, borrow;
    NN_UINT i;

    borrow = 0;

    for (i = 0; i < digits; i++) {
      if ((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
        ai = MAX_NN_DIGIT - c[i];
      else if ((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
        borrow = 1;
      else
        borrow = 0;
      a[i] = ai;
    }
    return borrow;
#endif

  }

  /* Computes a = b * c.
     a, b, c can be same
     Lengths: a[2*digits], b[digits], c[digits].
     Assumes digits < MAX_NN_DIGITS.
   */
  void NN_Mult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits) 
  {
	  watchdog_periodic();
    //#ifdef INLINE_ASM
#ifdef HYBRID_MULT
#ifdef CONTIKI_TARGET_MICAZ
#ifdef HYBRID_MUL_WIDTH4
    uint8_t n_d;
    
    n_d = digits/4;

    //r2~r10
    //r11~r14
    //r15
    //r16 i
    //r17 j
    //r19 0
    //r21:r20 b
    //r23:r22 c
    //r18 (i-j)*d ----------------
    //r20 j*d  ----------------
    //r25 d
    asm volatile (//"push r0 \n\t"
		  "push r1 \n\t"
		  "push r28 \n\t"
		  "push r29 \n\t"
		  "clr r2 \n\t"  //init 9 registers for accumulator
		  "clr r3 \n\t"
		  "clr r4 \n\t"
		  "clr r5 \n\t"
		  "clr r6 \n\t"
		  "clr r7 \n\t"
		  "clr r8 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"  //end of init
		  "clr r19 \n\t"  //zero
		  "ldi r25, 4 \n\t"  //d=4
		  "dec %3 \n\t"
		  "ldi r16, 0 \n\t"  //i
		  "LOOP1: ldi r17, 0 \n\t"  //j=0
		  "mul r16, r25 \n\t"
		  "add r0, r25 \n\t"
		  "movw r26, %A1 \n\t"
		  "add r26, r0 \n\t"
		  "adc r27, r1 \n\t"  //load b, (i-j+1)*d-1
		  "movw r28, %A2 \n\t"  //load c
		  "LOOP2: ld r14, -X \n\t"  //load b0~b(d-1)
		  "ld r13, -X \n\t"
		  "ld r12, -X \n\t"
		  "ld r11, -X \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+0]
		  "mul r11, r15 \n\t"  //t=0
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "brcc T01 \n\t"
		  "adc r4, r19 \n\t"
		  "brcc T01 \n\t"
		  "adc r5, r19 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T01: mul r12, r15 \n\t"  //t=1
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T02 \n\t"
		  "adc r5, r19 \n\t"
		  "brcc T02 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T02: mul r13, r15 \n\t"  //t=2
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T03 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc T03 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T03: mul r14, r15 \n\t"  //t=3
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T04 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T04 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T04: ld r15, Y+ \n\t"  //load c[j*d+1]
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T11 \n\t"
		  "adc r5, r19 \n\t"
		  "brcc T11 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T11: mul r12, r15 \n\t"  //t=1
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T12 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc T12 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T12: mul r13, r15 \n\t"  //t=2
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T13 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T13 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T13: mul r14, r15 \n\t"  //t=3
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc T14 \n\t"
		  "adc r8, r19 \n\t"
		  "brcc T14 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T14: ld r15, Y+ \n\t"  //load c[j*d+2]
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T21 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc T21 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T21: mul r12, r15 \n\t"  //t=1
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T22 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T22 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T22: mul r13, r15 \n\t"  //t=2
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc T23 \n\t"
		  "adc r8, r19 \n\t"
		  "brcc T23 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T23: mul r14, r15 \n\t"  //t=3
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T24 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T24: ld r15, Y+ \n\t"  //load c[j*d+3]
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T31 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T31 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T31: mul r12, r15 \n\t"  //t=1
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc T32 \n\t"
		  "adc r8, r19 \n\t"
		  "brcc T32 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T32: mul r13, r15 \n\t"  //t=2
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T33 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T33: mul r14, r15 \n\t"  //t=3
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "adc r10, r19 \n\t"
		  "cp r17, r16 \n\t"  //i == j?
		  "breq  LOOP2_EXIT \n\t"
		  "inc r17 \n\t"
		  "jmp LOOP2 \n\t"
		  "LOOP2_EXIT: st Z+, r2 \n\t"  //a[i*d] = r2
		  "st Z+, r3 \n\t"
		  "st Z+, r4 \n\t"
		  "st Z+, r5 \n\t"
		  "movw r2, r6 \n\t"  //can be speed up use movw
		  "movw r4, r8 \n\t"
		  "mov r6, r10 \n\t"  //can be remove
		  "clr r7 \n\t"
		  "clr r8 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"
		  "cp r16, %3 \n\t"  //i == 4?
		  "breq LOOP1_EXIT \n\t"
		  "inc r16 \n\t"
		  "jmp LOOP1 \n\t"
		  "LOOP1_EXIT: inc r16 \n\t"  //i = 5
		  "LOOP3: mov r17, r16 \n\t"  //j = i-4
		  "sub r17, %3 \n\t"
		  "mul r25, %3 \n\t"
		  "add r0, r25 \n\t"
		  "movw r26, %A1 \n\t"
		  "add r26, r0 \n\t"
		  "adc r27, r1 \n\t"  //load b
		  "mul r17, r25 \n\t"  //j*d
		  "movw r28, %A2 \n\t"
		  "add r28, r0 \n\t"
		  "adc r29, r1 \n\t"  //load c
		  "LOOP4: ld r14, -X \n\t"  //load b0~b(d-1)
		  "ld r13, -X \n\t"
		  "ld r12, -X \n\t"
		  "ld r11, -X \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+0]
		  "mul r11, r15 \n\t"  //t=0
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "brcc T41 \n\t"
		  "adc r4, r19 \n\t"
		  "brcc T41 \n\t"
		  "adc r5, r19 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T41: mul r12, r15 \n\t"  //t=1
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T42 \n\t"
		  "adc r5, r19 \n\t"
		  "brcc T42 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T42: mul r13, r15 \n\t"  //t=2
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T43 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc T43 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T43: mul r14, r15 \n\t"  //t=3
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T44 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T44 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T44: ld r15, Y+ \n\t"  //load c
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T51 \n\t"
		  "adc r5, r19 \n\t"
		  "brcc T51 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T51: mul r12, r15 \n\t"  //t=1
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T52 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc T52 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T52: mul r13, r15 \n\t"  //t=2
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T53 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T53 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T53: mul r14, r15 \n\t"  //t=3
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc T54 \n\t"
		  "adc r8, r19 \n\t"
		  "brcc T54 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T54: ld r15, Y+ \n\t"  //load c
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T61 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc T61 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T61: mul r12, r15 \n\t"  //t=1
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T62 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T62 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T62: mul r13, r15 \n\t"  //t=2
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc T63 \n\t"
		  "adc r8, r19 \n\t"
		  "brcc T63 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T63: mul r14, r15 \n\t"  //t=3
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T64 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T64: ld r15, Y+ \n\t"  //load c
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T71 \n\t"
		  "adc r7, r19 \n\t"
		  "brcc T71 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T71: mul r12, r15 \n\t"  //t=1
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc T72 \n\t"
		  "adc r8, r19 \n\t"
		  "brcc T72 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T72: mul r13, r15 \n\t"  //t=2
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T73 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "T73: mul r14, r15 \n\t"  //t=3
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "adc r10, r19 \n\t"
		  "cp r17, %3 \n\t"  //j=4?
		  "breq  LOOP4_EXIT \n\t"
		  "inc r17 \n\t"
		  "jmp LOOP4 \n\t"
		  "LOOP4_EXIT: st Z+, r2 \n\t"  //a[i*d] = r2
		  "st Z+, r3 \n\t"
		  "st Z+, r4 \n\t"
		  "st Z+, r5 \n\t"
		  "movw r2, r6 \n\t"  //can be speed up use movw
		  "movw r4, r8 \n\t"
		  "mov r6, r10 \n\t"  //can be remove
		  "clr r7 \n\t"
		  "clr r8 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"
		  "mov r0, %3 \n\t"
		  "lsl r0 \n\t"
		  "cp r16, r0 \n\t"
		  "breq LOOP3_EXIT \n\t"
		  "inc r16 \n\t"
		  "jmp LOOP3 \n\t"
		  "LOOP3_EXIT: st Z+, r2 \n\t"
		  "st Z+, r3 \n\t"
		  "st Z+, r4 \n\t"
		  "st Z+, r5 \n\t"
		  "pop r29 \n\t"
		  "pop r28 \n\t"
		  "pop r1 \n\t"
		  //"pop r0 \n\t"
		  :
		  :"z"(a),"a"(b),"a"(c),"r"(n_d)
		  :"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","r15","r16","r17","r19","r25","r26","r27","r28","r29"
		  );

#else
    uint8_t n_d;

    n_d = digits/5;
    //n_d = 4;
    //r2~r6 r8~r12 r25  acumulator
    //r13~r17  save b
    //r18  c
    //r19  i
    //r20  j
    //r21  0
    //r22  d
    //r24  2*(n/d)-2

    asm volatile ("push r0 \n\t"
		  "push r1 \n\t"
		  "push r28 \n\t"
		  "push r29 \n\t"
		  "clr r2 \n\t"  //init 11 registers for accumulator
		  "clr r3 \n\t"
		  "clr r4 \n\t"
		  "clr r5 \n\t"
		  "clr r6 \n\t"
		  "clr r8 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"
		  "clr r11 \n\t"
		  "clr r12 \n\t"
		  "clr r25 \n\t"//end of init
		  "clr r21 \n\t"  //r21=0
		  "ldi r22, 5 \n\t"  //r22=d=5
		  "dec %3 \n\t"
		  "mov r24, %3 \n\t"
		  "lsl r24 \n\t"
		  "ldi r19, 0 \n\t"  //i
		  "LOOP1: ldi r20, 0 \n\t"  //j
		  "mul r19, r22 \n\t"
		  "add r0, r22 \n\t"
		  "add %A1, r0 \n\t"
		  "adc %B1, r1 \n\t"  //load b
		  "LOOP2: ld r17, -%a1 \n\t"  //load b0~b(d-1)
		  "ld r16, -%a1 \n\t"
		  "ld r15, -%a1 \n\t"
		  "ld r14, -%a1 \n\t"
		  "ld r13, -%a1 \n\t"
		  "ld r18, %a2+ \n\t"
		  "mul r13, r18 \n\t"  //t=0
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "brcc T01 \n\t"
		  "adc r4, r21 \n\t"
		  "brcc T01 \n\t"
		  "adc r5, r21 \n\t"
		  "adc r6, r21 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T01: mul r14, r18 \n\t"  //t=1
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T02 \n\t"
		  "adc r5, r21 \n\t"
		  "brcc T02 \n\t"
		  "adc r6, r21 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T02: mul r15, r18 \n\t"  //t=2
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T03 \n\t"
		  "adc r6, r21 \n\t"
		  "brcc T03 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T03: mul r16, r18 \n\t"  //t=3
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T04 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T04 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T04: mul r17, r18 \n\t"  //t=4
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T05 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T05 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T05: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T11 \n\t"
		  "adc r5, r21 \n\t"
		  "brcc T11 \n\t"
		  "adc r6, r21 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T11: mul r14, r18 \n\t"  //t=1
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T12 \n\t"
		  "adc r6, r21 \n\t"
		  "brcc T12 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T12: mul r15, r18 \n\t"  //t=2
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T13 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T13 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T13: mul r16, r18 \n\t"  //t=3
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T14 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T14 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T14: mul r17, r18 \n\t"  //t=4
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T15 \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T15 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T15: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T21 \n\t"
		  "adc r6, r21 \n\t"
		  "brcc T21 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T21: mul r14, r18 \n\t"  //t=1
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T22 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T22 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T22: mul r15, r18 \n\t"  //t=2
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T23 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T23 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T23: mul r16, r18 \n\t"  //t=3
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T24 \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T24 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T24: mul r17, r18 \n\t"  //t=4
		  "add r9, r0 \n\t"
		  "adc r10, r1 \n\t"
		  "brcc T25 \n\t"
		  "adc r11, r21 \n\t"
		  "brcc T25 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T25: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T31 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T31 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T31: mul r14, r18 \n\t"  //t=1
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T32 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T32 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T32: mul r15, r18 \n\t"  //t=2
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T33 \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T33 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T33: mul r16, r18 \n\t"  //t=3
		  "add r9, r0 \n\t"
		  "adc r10, r1 \n\t"
		  "brcc T34 \n\t"
		  "adc r11, r21 \n\t"
		  "brcc T34 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T34: mul r17, r18 \n\t"  //t=4
		  "add r10, r0 \n\t"
		  "adc r11, r1 \n\t"
		  "brcc T35 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T35: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T41P \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T41P \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T41P: mul r14, r18 \n\t"  //t=1
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T42P \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T42P \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T42P: mul r15, r18 \n\t"  //t=2
		  "add r9, r0 \n\t"
		  "adc r10, r1 \n\t"
		  "brcc T43P \n\t"
		  "adc r11, r21 \n\t"
		  "brcc T43P \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T43P: mul r16, r18 \n\t"  //t=3
		  "add r10, r0 \n\t"
		  "adc r11, r1 \n\t"
		  "brcc T44P \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T44P: mul r17, r18 \n\t"  //t=4
		  "add r11, r0 \n\t"
		  "adc r12, r1 \n\t"
		  "adc r25, r21 \n\t"
		  "cp r20, r19 \n\t"  //i == j?
		  "breq  LOOP2_EXIT \n\t"
		  "inc r20 \n\t"
		  "jmp LOOP2 \n\t"
		  "LOOP2_EXIT: st %a0+, r2 \n\t"  //a[i*d] = r2
		  "st %a0+, r3 \n\t"
		  "st %a0+, r4 \n\t"
		  "st %a0+, r5 \n\t"
		  "st %a0+, r6 \n\t"
		  "movw r2, r8 \n\t"
		  "movw r4, r10 \n\t"
		  "mov r6, r12 \n\t"
		  "mov r8, r25 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"
		  "clr r11 \n\t"
		  "clr r12 \n\t"
		  "clr r25 \n\t"
		  "mul r19, r22 \n\t"  //restore c base address
		  "add r0, r22 \n\t"
		  "sub %A2, r0 \n\t"
		  "sbc %B2, r1 \n\t"
		  "cp r19, %3 \n\t"  //i == ceiling(n/d)-1?
		  "breq LOOP1_EXIT \n\t"
		  "inc r19 \n\t"
		  "jmp LOOP1 \n\t"
		  "LOOP1_EXIT: inc r19 \n\t"  //i = 5
		  "add %A2, r22 \n\t"
		  "adc %B2, r21 \n\t"  //init base address c
		  "mul %3, r22 \n\t"
		  "add r0, r22 \n\t"
		  "add %A1, r0 \n\t"
		  "adc %B1, r1 \n\t"  //load b
		  "LOOP3: mov r20, r19 \n\t"  //j = i-4
		  "sub r20, %3 \n\t"
		  "LOOP4: ld r17, -%a1 \n\t"  //load b0~b(d-1)
		  "ld r16, -%a1 \n\t"
		  "ld r15, -%a1 \n\t"
		  "ld r14, -%a1 \n\t"
		  "ld r13, -%a1 \n\t"
		  "ld r18, %a2+ \n\t"
		  "mul r13, r18 \n\t"  //t=0
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "brcc T41 \n\t"
		  "adc r4, r21 \n\t"
		  "brcc T41 \n\t"
		  "adc r5, r21 \n\t"
		  "adc r6, r21 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T41: mul r14, r18 \n\t"  //t=1
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T42 \n\t"
		  "adc r5, r21 \n\t"
		  "brcc T42 \n\t"
		  "adc r6, r21 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T42: mul r15, r18 \n\t"  //t=2
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T43 \n\t"
		  "adc r6, r21 \n\t"
		  "brcc T43 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T43: mul r16, r18 \n\t"  //t=3
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T44 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T44 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T44: mul r17, r18 \n\t"  //t=4
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T45 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T45 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T45: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "brcc T51 \n\t"
		  "adc r5, r21 \n\t"
		  "brcc T51 \n\t"
		  "adc r6, r21 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T51: mul r14, r18 \n\t"  //t=1
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T52 \n\t"
		  "adc r6, r21 \n\t"
		  "brcc T52 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T52: mul r15, r18 \n\t"  //t=2
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T53 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T53 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T53: mul r16, r18 \n\t"  //t=3
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T54 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T54 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T54: mul r17, r18 \n\t"
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T55 \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T55 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T55: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "brcc T61 \n\t"
		  "adc r6, r21 \n\t"
		  "brcc T61 \n\t"
		  "adc r8, r21 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T61: mul r14, r18 \n\t"  //t=1
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T62 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T62 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T62: mul r15, r18 \n\t"  //t=2
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T63 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T63 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T63: mul r16, r18 \n\t"  //t=3
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T64 \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T64 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T64: mul r17, r18 \n\t"
		  "add r9, r0 \n\t"
		  "adc r10, r1 \n\t"
		  "brcc T65 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T65: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "brcc T71 \n\t"
		  "adc r8, r21 \n\t"
		  "brcc T71 \n\t"
		  "adc r9, r21 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T71: mul r14, r18 \n\t"  //t=1
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T72 \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T72 \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T72: mul r15, r18 \n\t"  //t=2
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T73 \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T73 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T73: mul r16, r18 \n\t"  //t=3
		  "add r9, r0 \n\t"
		  "adc r10, r1 \n\t"
		  "brcc T74 \n\t"
		  "adc r11, r21 \n\t"
		  "brcc T74 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T74: mul r17, r18 \n\t"  //t=4
		  "add r10, r0 \n\t"
		  "adc r11, r1 \n\t"
		  "brcc T75 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T75: ld r18, %a2+ \n\t"  //load c
		  "mul r13, r18 \n\t" //t=0, b0*c
		  "add r6, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "brcc T71P \n\t"
		  "adc r9, r21 \n\t"
		  "brcc T71P \n\t"
		  "adc r10, r21 \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T71P: mul r14, r18 \n\t"  //t=1
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "brcc T72P \n\t"
		  "adc r10, r21 \n\t"
		  "brcc T72P \n\t"
		  "adc r11, r21 \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T72P: mul r15, r18 \n\t"  //t=2
		  "add r9, r0 \n\t"
		  "adc r10, r1 \n\t"
		  "brcc T73P \n\t"
		  "adc r11, r21 \n\t"
		  "brcc T73P \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T73P: mul r16, r18 \n\t"  //t=3
		  "add r10, r0 \n\t"
		  "adc r11, r1 \n\t"
		  "brcc T74P \n\t"
		  "adc r12, r21 \n\t"
		  "adc r25, r21 \n\t"
		  "T74P: mul r17, r18 \n\t"  //t=4
		  "add r11, r0 \n\t"
		  "adc r12, r1 \n\t"
		  "adc r25, r21 \n\t"
		  "cp r20, %3 \n\t"  //j=ceiling(n/d)-1?
		  "breq  LOOP4_EXIT \n\t"
		  "inc r20 \n\t"
		  "jmp LOOP4 \n\t"
		  "LOOP4_EXIT: st %a0+, r2 \n\t"  //a[i*d] = r2
		  "st %a0+, r3 \n\t"
		  "st %a0+, r4 \n\t"
		  "st %a0+, r5 \n\t"
		  "st %a0+, r6 \n\t"
		  "movw r2, r8 \n\t"
		  "movw r4, r10 \n\t"
		  "mov r6, r12 \n\t"
		  "mov r8, r25 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"
		  "clr r11 \n\t"
		  "clr r12 \n\t"
		  "clr r25 \n\t"
		  "mov r0, r24 \n\t"
		  "sub r0, r19 \n\t"
		  "mul r0, r22 \n\t"
		  "sub %A2, r0 \n\t"
		  "sbc %B2, r1 \n\t"  //restore c
		  "add r0, r22 \n\t"
		  "add %A1, r0 \n\t"
		  "adc %B1, r1 \n\t"  //restore b
		  "cp r19, r24 \n\t"
		  "breq LOOP3_EXIT \n\t"
		  "inc r19 \n\t"
		  "jmp LOOP3 \n\t"
		  "LOOP3_EXIT: st %a0+, r2 \n\t"
		  "st %a0+, r3 \n\t"
		  "st %a0+, r4 \n\t"
		  "st %a0+, r5 \n\t"
		  "st %a0+, r6 \n\t"
		  "pop r29 \n\t"
		  "pop r28 \n\t"
		  "pop r1 \n\t"
		  "pop r0 \n\t"
		  :
		  :"e"(a),"e"(b),"e"(c),"a"(n_d)
		  :"r0","r1","r2","r3","r4","r5","r6","r8","r9","r10","r11","r12","r13","r14","r15","r16","r17","r18","r19","r20","r21","r22","r24","r25"
		  );
#endif
#endif //CONTIKI_TARGET_MICAZ

#ifdef CONTIKI_TARGET_SKY

    //r9~r11 accumulator
    //r12 i
    //r13 j
    //r14 address of b, for m_a
    //r15 address of c, for m_b

    asm volatile ("clr r9 \n\t" //init accumulator
		  "clr r10 \n\t"
		  "clr r11 \n\t"
		  "clr r12 \n\t" //i=0
		  "dec %3 \n\t"  //digits--
		  "LOOP1: clr r13 \n\t" //j=0
		  "mov r12, r14 \n\t"
		  "inc r14 \n\t"
		  "add r14, r14 \n\t"
		  "add %1, r14 \n\t" //address is ready for m_a
		  "LOOP2: sub #2, r14 \n\t"
		  "mov r13, r15 \n\t"
		  "add r15, r15 \n\t"
		  "add %2, r15 \n\t"  //address is ready for m_b
		  "mov @r14, &0x130 \n\t"
		  "mov @r15, &0x138 \n\t"  //load m_c, get mul result
		  "add &0x13A, r9 \n\t" //add to accumulator
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "cmp r12, r13 \n\t"  //compare i and j
		  "jeq LOOP2_EXIT \n\t"
		  "inc r13 \n\t"
		  "jmp LOOP2 \n\t"
		  "LOOP2_EXIT: mov r9, 0(%0) \n\t"
		  "add #2, %0 \n\t"
		  "mov r10, r9 \n\t"
		  "mov r11, r10 \n\t"
		  "clr r11 \n\t"
		  "cmp %3, r12 \n\t"
		  "jeq LOOP1_EXIT \n\t"
		  "inc r12 \n\t"  //i++
		  "jmp LOOP1 \n\t"
		  "LOOP1_EXIT: inc r12 \n\t" //i=[n/d]
		  "LOOP3: mov r12, r13 \n\t"
		  "sub %3, r13 \n\t" //j=i-[n/d]
		  "mov %3, r14 \n\t"
		  "inc r14 \n\t"
		  "add r14, r14 \n\t"
		  "add %1, r14 \n\t"  //address is ready for m_a
		  "LOOP4: sub #2, r14 \n\t"
		  "mov r13, r15 \n\t"
		  "add r15, r15 \n\t"
		  "add %2, r15 \n\t"
		  "mov @r14, &0x130 \n\t"
		  "mov @r15, &0x138 \n\t"
		  "add &0x13A, r9 \n\t"
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "cmp %3, r13 \n\t"
		  "jeq LOOP4_EXIT \n\t"
		  "inc r13 \n\t"
		  "jmp LOOP4 \n\t"
		  "LOOP4_EXIT: mov r9, 0(%0) \n\t"
		  "add #2, %0 \n\t"
		  "mov r10, r9 \n\t"
		  "mov r11, r10 \n\t"
		  "mov #0, r11 \n\t"
		  "mov %3, r15 \n\t"
		  "add r15, r15 \n\t"
		  "cmp r12, r15 \n\t"
		  "jeq LOOP3_EXIT \n\t"
		  "inc r12 \n\t"
		  "jmp LOOP3 \n\t"
		  "LOOP3_EXIT:mov r9, 0(%0) \n\t"
		  :
		  :"r"(a), "r"(b), "r"(c), "r"(digits)
		  :"r9", "r10", "r11", "r12", "r13", "r14", "r15"
		  );

    /*
    //r4~r6 accumulator
    //r7 m_a
    //r8 m_b
    //r9 i
    //r10 j
    //r12 address of b, for m_a
    //r13 address of c, for m_b


    asm volatile ("push r4 \n\t"
		  "clr r4 \n\t" //init accumulator
		  "clr r5 \n\t"
		  "clr r6 \n\t"
		  "clr r9 \n\t" //i=0
		  "dec %3 \n\t"
		  "LOOP1: clr r10 \n\t" //j=0
		  "mov r9, r12 \n\t"
		  "inc r12 \n\t"
		  "add r12, r12 \n\t"
		  "add %1, r12 \n\t" //address is ready for m_a
		  "LOOP2: sub #2, r12 \n\t"
		  //"mov @r12, r7 \n\t"  //load m_a
		  "mov r10, r13 \n\t"
		  "add r13, r13 \n\t"
		  "add %2, r13 \n\t"  //address is ready for m_b
		  //"mov r7, &0x130 \n\t"
		  "mov @r12, &0x130 \n\t"
		  "mov @r13, &0x138 \n\t"  //load m_c, get mul result
		  "add &0x13A, r4 \n\t" //add to accumulator
		  "addc &0x13C, r5 \n\t"
		  "addc #0, r6 \n\t"
		  "cmp r9, r10 \n\t"  //compare i and j
		  "jeq LOOP2_EXIT \n\t"
		  "inc r10 \n\t"
		  "jmp LOOP2 \n\t"
		  "LOOP2_EXIT: mov r4, 0(%0) \n\t"
		  "add #2, %0 \n\t"
		  "mov r5, r4 \n\t"
		  "mov r6, r5 \n\t"
		  "clr r6 \n\t"
		  "cmp %3, r9 \n\t"
		  "jeq LOOP1_EXIT \n\t"
		  "inc r9 \n\t"  //i++
		  "jmp LOOP1 \n\t"
		  "LOOP1_EXIT: inc r9 \n\t" //i=[n/d]
		  "LOOP3: mov r9, r10 \n\t"
		  "sub %3, r10 \n\t" //j=i-[n/d]
		  "mov %3, r12 \n\t"
		  "inc r12 \n\t"
		  "add r12, r12 \n\t"
		  "add %1, r12 \n\t"  //address is ready for m_a
		  "LOOP4: sub #2, r12 \n\t"
		  //"mov @r12, r7 \n\t"  //load m_a
		  "mov r10, r13 \n\t"
		  "add r13, r13 \n\t"
		  "add %2, r13 \n\t"
		  //"mov r7, &0x130 \n\t"
		  "mov @r12, &0x130 \n\t"
		  "mov @r13, &0x138 \n\t"
		  "add &0x13A, r4 \n\t"
		  "addc &0x13C, r5 \n\t"
		  "addc #0, r6 \n\t"
		  "cmp %3, r10 \n\t"
		  "jeq LOOP4_EXIT \n\t"
		  "inc r10 \n\t"
		  "jmp LOOP4 \n\t"
		  "LOOP4_EXIT: mov r4, 0(%0) \n\t"
		  "add #2, %0 \n\t"
		  "mov r5, r4 \n\t"
		  "mov r6, r5 \n\t"
		  "mov #0, r6 \n\t"
		  "mov %3, r13 \n\t"
		  "add r13, r13 \n\t"
		  "cmp r9, r13 \n\t"
		  "jeq LOOP3_EXIT \n\t"
		  "inc r9 \n\t"
		  "jmp LOOP3 \n\t"
		  "LOOP3_EXIT:mov r4, 0(%0) \n\t"
		  "pop r4 \n\t"
		  :
		  :"r"(a), "r"(b), "r"(c), "r"(digits)
		  :"r4", "r5", "r6", "r9", "r10", "r12", "r13"
		  );
    */
    /*
    NN_DIGIT t[2*MAX_NN_DIGITS];
    NN_UINT bDigits, cDigits, i;

    NN_AssignZero (t, 2 * digits);
  
    bDigits = NN_Digits (b, digits);
    cDigits = NN_Digits (c, digits);

    for (i = 0; i < bDigits; i++)
      t[i+cDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], c, cDigits);
  
    NN_Assign (a, t, 2 * digits);
    */
#endif //CONTIKI_TARGET_SKY

#ifdef CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki
    //r0~r2
    //r3 b
    //r4 i
    //r5 j
    //r6 address for b
    //r7 tmp, address for c
    //r8 tmp, c

    asm volatile ("STMFD r13!, {r0 - r8} \n\t"
		  "MOV r0, #0 \n\t"  //init accumulator
		  "MOV r1, #0 \n\t"
		  "MOV r2, #0 \n\t"  //end of init
		  "SUB %3, %3, #1 \n\t"  //[n/d]-1
		  "MOV r4, #0 \n\t"  //i
		  "LOOP1: MOV r5, #0 \n\t"  //j=0
		  "ADD r7, r4, #1 \n\t"  //(i+1)
		  "ADD r6, %1, r7, lsl #2 \n\t"  //address is ready for b
		  "LOOP2: LDR r3, [r6, #-4]! \n\t"  //load b0~b(d-1)
		  "ADD r7, %2, r5, lsl #2 \n\t"  //compute address for c
		  "LDR r8, [r7] \n\t"  //load c[j*d]  //compute address for c
		  "UMULL r7, r8, r3, r8 \n\t"  //t=0
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "CMP r4, r5\n\t"  //i==j?
		  "BEQ LOOP2_EXIT \n\t"
		  "ADD r5, r5, #1 \n\t" //j++
		  "B LOOP2 \n\t"
		  "LOOP2_EXIT: STR r0, [%0], #4 \n\t"  //a[i*d] = r0
		  "MOV r0, r1 \n\t"
		  "MOV r1, r2 \n\t"
		  "MOV r2, #0 \n\t"
		  "CMP r4, %3 \n\t"
		  "BEQ LOOP1_EXIT \n\t"
		  "ADD r4, r4, #1 \n\t" //i++
		  "B LOOP1 \n\t"
		  "LOOP1_EXIT: ADD r4, r4, #1 \n\t"  //i = [n/d]
		  "LOOP3: SUB r5, r4, %3 \n\t"  //j=i-[n/d]
		  "ADD r7, %3, #1 \n\t"
		  "ADD r6, %1, r7, lsl #2 \n\t"  //address for b is ready
		  "LOOP4: LDR r3, [r6, #-4]! \n\t"  //load b0~b[d-1]
		  "ADD r7, %2, r5, lsl #2 \n\t"  //compute address for c
		  "LDR r8, [r7] \n\t"		  
		  "UMULL r7, r8, r3, r8 \n\t"  //t=0
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "CMP r5, %3 \n\t"  //j==[n/d]?
		  "BEQ LOOP4_EXIT \n\t"
		  "ADD r5, r5, #1 \n\t"  //j++
		  "B LOOP4 \n\t"
		  "LOOP4_EXIT: STR r0, [%0], #4 \n\t"  //a[i*d] = r0
		  "MOV r0, r1 \n\t"
		  "MOV r1, r2 \n\t"
		  "MOV r2, #0 \n\t"
		  "CMP r4, %3, lsl #1 \n\t"
		  "BEQ LOOP3_EXIT \n\t"
		  "ADD r4, r4, #1 \n\t"
		  "B LOOP3 \n\t"
		  "LOOP3_EXIT: STR r0, [%0], #4 \n\t"
		  "LDMFD r13!, {r0 - r8} \n\t"
		  :
		  :"r"(a), "r"(b), "r"(c), "r"(digits)
		  :"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8"
		  );
#endif //CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki

#else  //no inline assembly

    NN_DIGIT t[2*MAX_NN_DIGITS+2];
    NN_UINT bDigits, cDigits, i;

    NN_AssignZero (t, 2 * digits);
  
    bDigits = NN_Digits (b, digits);
    cDigits = NN_Digits (c, digits);

    for (i = 0; i < bDigits; i++)
      t[i+cDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], c, cDigits);
  
    NN_Assign (a, t, 2 * digits);

#endif //INLINE_ASM
  }


  void NN_Sqr(NN_DIGIT *a, NN_DIGIT *b, NN_UINT digits)
  {
	  watchdog_periodic();
    //#ifdef INLINE_ASM
#ifdef HYBRID_SQR
#ifdef CONTIKI_TARGET_MICAZ
    uint8_t n_d;
    
    n_d = digits/4;
    
    //r2~r10
    //r11~r14
    //r15
    //r16 i
    //r17 j
    //r19 0
    //r21:r20 b
    //r25 d
    asm volatile (//"push r0 \n\t"
		  "push r1 \n\t"
		  "push r28 \n\t"
		  "push r29 \n\t"
		  "clr r2 \n\t"  //init 9 registers for accumulator
		  "clr r3 \n\t"
		  "clr r4 \n\t"
		  "clr r5 \n\t"
		  "clr r6 \n\t"
		  "clr r7 \n\t"
		  "clr r8 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"  //end of init
		  "clr r19 \n\t"  //zero
		  "ldi r25, 4 \n\t"  //d=4
		  "dec %2 \n\t"
		  "ldi r16, 0 \n\t"  //i
		  "SQR_LOOP1: ldi r17, 0 \n\t"  //j=0
		  "mul r16, r25 \n\t"
		  "add r0, r25 \n\t"
		  "movw r26, %A1 \n\t"
		  "add r26, r0 \n\t"
		  "adc r27, r1 \n\t"  //load b, (i-j+1)*d-1
		  "movw r28, %A1 \n\t"  //load c
		  "SQR_LOOP2: mov r0, r16 \n\t"
		  "sub r0, r17 \n\t"
		  "cp r0, r17 \n\t"
		  "breq JMP_EQ_1 \n\t"
		  "brlo JMP_SQR_LOOP2_EXIT \n\t"
		  "jmp SQR_LOOP2_1 \n\t"
		  "JMP_EQ_1: jmp EQ_1 \n\t"
		  "JMP_SQR_LOOP2_EXIT: jmp SQR_LOOP2_EXIT \n\t"
		  "SQR_LOOP2_1: ld r14, -X \n\t"  //load b0~b(d-1)
		  "ld r13, -X \n\t"
		  "ld r12, -X \n\t"
		  "ld r11, -X \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+0]
		  "mul r11, r15 \n\t"  //t=0
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "adc r4, r24 \n\t"
		  "brcc SQR_T01 \n\t"
		  "adc r5, r19 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T01: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "adc r5, r24 \n\t"
		  "brcc SQR_T02 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T02: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc SQR_T03 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T03: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T04 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T04: ld r15, Y+ \n\t"  //load c[j*d+1]
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "adc r5, r24 \n\t"
		  "brcc SQR_T11 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T11: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc SQR_T12 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T12: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T13 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T13: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc SQR_T14 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T14: ld r15, Y+ \n\t"  //load c[j*d+2]
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc SQR_T21 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T21: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T22 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T22: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc SQR_T23 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T23: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "adc r9, r24 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T24: ld r15, Y+ \n\t"  //load c[j*d+3]
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T31 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T31: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc SQR_T32 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T32: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "adc r9, r24 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T33: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "adc r10, r24 \n\t"
		  "inc r17 \n\t"  //j++
		  "jmp SQR_LOOP2 \n\t"
		  "EQ_1: ld r14, -X \n\t"  //load b0~b(d-1)
		  "ld r13, -X \n\t"
		  "ld r12, -X \n\t"
		  "ld r11, -X \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+0]
		  "mul r11, r15 \n\t"  //t=0
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "adc r4, r19 \n\t"
		  "brcc EQ_SQR_T01 \n\t"
		  "adc r5, r19 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T01: mul r12, r15 \n\t"
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "adc r5, r24 \n\t"
		  "brcc EQ_SQR_T02 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T02: mul r13, r15 \n\t"
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc EQ_SQR_T03 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T03: mul r14, r15 \n\t"
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc EQ_SQR_T04 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T04: ld r15, Y+ \n\t"  //load c[j*d+1]
		  "mul r12, r15 \n\t"  //t=1
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc EQ_SQR_T12 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T12: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc EQ_SQR_T13 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T13: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc EQ_SQR_T14 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T14: ld r15, Y+ \n\t"  //load c[j*d+2]
		  "mul r13, r15 \n\t"  //t=2
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc EQ_SQR_T23 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T23: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "adc r9, r24 \n\t"
		  "adc r10, r19 \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+3]
		  "mul r14, r15 \n\t"  //t=3
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_LOOP2_EXIT: st Z+, r2 \n\t"  //a[i*d] = r2
		  "st Z+, r3 \n\t"
		  "st Z+, r4 \n\t"
		  "st Z+, r5 \n\t"
		  "movw r2, r6 \n\t"  //can be speed up use movw
		  "movw r4, r8 \n\t"
		  "mov r6, r10 \n\t"  //can be remove
		  "clr r7 \n\t"
		  "clr r8 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"
		  "cp r16, %2 \n\t"  //i == 4?
		  "breq SQR_LOOP1_EXIT \n\t"
		  "inc r16 \n\t"
		  "jmp SQR_LOOP1 \n\t"
		  "SQR_LOOP1_EXIT: inc r16 \n\t"  //i = 5
		  "SQR_LOOP3: mov r17, r16 \n\t"  //j = i-4
		  "sub r17, %2 \n\t"
		  "mul r25, %2 \n\t"
		  "add r0, r25 \n\t"
		  "movw r26, %A1 \n\t"
		  "add r26, r0 \n\t"
		  "adc r27, r1 \n\t"  //load b
		  "mul r17, r25 \n\t"  //j*d
		  "movw r28, %A1 \n\t"
		  "add r28, r0 \n\t"
		  "adc r29, r1 \n\t"  //load c
		  "SQR_LOOP4: mov r0, r16 \n\t"
		  "sub r0, r17 \n\t"
		  "cp r0, r17 \n\t"
		  "breq JMP_EQ_2 \n\t"
		  "brlo JMP_SQR_LOOP4_EXIT \n\t"
		  "jmp SQR_LOOP4_1 \n\t"
		  "JMP_EQ_2: jmp EQ_2 \n\t"
		  "JMP_SQR_LOOP4_EXIT: jmp SQR_LOOP4_EXIT \n\t"
		  "SQR_LOOP4_1: ld r14, -X \n\t"  //load b0~b(d-1)
		  "ld r13, -X \n\t"
		  "ld r12, -X \n\t"
		  "ld r11, -X \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+0]
		  "mul r11, r15 \n\t"  //t=0
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "adc r4, r24 \n\t"
		  "brcc SQR_T41 \n\t"
		  "adc r5, r19 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T41: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "adc r5, r24 \n\t"
		  "brcc SQR_T42 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T42: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc SQR_T43 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T43: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T44 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T44: ld r15, Y+ \n\t"  //load c
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "adc r5, r24 \n\t"
		  "brcc SQR_T51 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T51: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc SQR_T52 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T52: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T53 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T53: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc SQR_T54 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T54: ld r15, Y+ \n\t"  //load c
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc SQR_T61 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T61: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T62 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T62: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc SQR_T63 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T63: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "adc r9, r24 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T64: ld r15, Y+ \n\t"  //load c
		  "mul r11, r15 \n\t" //t=0, b0*c
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc SQR_T71 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T71: mul r12, r15 \n\t"  //t=1
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc SQR_T72 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T72: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "adc r9, r24 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_T73: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "adc r10, r24 \n\t"
		  "inc r17 \n\t"
		  "jmp SQR_LOOP4 \n\t"
		  "EQ_2: ld r14, -X \n\t"  //load b0~b(d-1)
		  "ld r13, -X \n\t"
		  "ld r12, -X \n\t"
		  "ld r11, -X \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+0]
		  "mul r11, r15 \n\t"  //t=0
		  "add r2, r0 \n\t"
		  "adc r3, r1 \n\t"
		  "adc r4, r19 \n\t"
		  "brcc EQ_SQR_T41 \n\t"
		  "adc r5, r19 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T41: mul r12, r15 \n\t"
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r3, r0 \n\t"
		  "adc r4, r1 \n\t"
		  "adc r5, r24 \n\t"
		  "brcc EQ_SQR_T42 \n\t"
		  "adc r6, r19 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T42: mul r13, r15 \n\t"
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r24 \n\t"
		  "brcc EQ_SQR_T43 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T43: mul r14, r15 \n\t"
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc EQ_SQR_T44 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T44: ld r15, Y+ \n\t"  //load c[j*d+1]
		  "mul r12, r15 \n\t"  //t=1
		  "add r4, r0 \n\t"
		  "adc r5, r1 \n\t"
		  "adc r6, r19 \n\t"
		  "brcc EQ_SQR_T52 \n\t"
		  "adc r7, r19 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T52: mul r13, r15 \n\t"  //t=2
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r5, r0 \n\t"
		  "adc r6, r1 \n\t"
		  "adc r7, r24 \n\t"
		  "brcc EQ_SQR_T53 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T53: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "adc r8, r24 \n\t"
		  "brcc EQ_SQR_T54 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T54: ld r15, Y+ \n\t"  //load c[j*d+2]
		  "mul r13, r15 \n\t"  //t=2
		  "add r6, r0 \n\t"
		  "adc r7, r1 \n\t"
		  "brcc EQ_SQR_T63 \n\t"
		  "adc r8, r19 \n\t"
		  "adc r9, r19 \n\t"
		  "adc r10, r19 \n\t"
		  "EQ_SQR_T63: mul r14, r15 \n\t"  //t=3
		  "clr r24 \n\t"
		  "lsl r0 \n\t"
		  "rol r1 \n\t"
		  "rol r24 \n\t"
		  "add r7, r0 \n\t"
		  "adc r8, r1 \n\t"
		  "adc r9, r24 \n\t"
		  "adc r10, r19 \n\t"
		  "ld r15, Y+ \n\t"  //load c[j*d+3]
		  "mul r14, r15 \n\t"  //t=3
		  "add r8, r0 \n\t"
		  "adc r9, r1 \n\t"
		  "adc r10, r19 \n\t"
		  "SQR_LOOP4_EXIT: st Z+, r2 \n\t"  //a[i*d] = r2
		  "st Z+, r3 \n\t"
		  "st Z+, r4 \n\t"
		  "st Z+, r5 \n\t"
		  "movw r2, r6 \n\t"  //can be speed up use movw
		  "movw r4, r8 \n\t"
		  "mov r6, r10 \n\t"  //can be remove
		  "clr r7 \n\t"
		  "clr r8 \n\t"
		  "clr r9 \n\t"
		  "clr r10 \n\t"
		  "mov r0, %2 \n\t"
		  "lsl r0 \n\t"
		  "cp r16, r0 \n\t"
		  "breq SQR_LOOP3_EXIT \n\t"
		  "inc r16 \n\t"
		  "jmp SQR_LOOP3 \n\t"
		  "SQR_LOOP3_EXIT: st Z+, r2 \n\t"
		  "st Z+, r3 \n\t"
		  "st Z+, r4 \n\t"
		  "st Z+, r5 \n\t"
		  "pop r29 \n\t"
		  "pop r28 \n\t"
		  "pop r1 \n\t"
		  //"pop r0 \n\t"
		  :
		  :"z"(a),"a"(b),"r"(n_d)
		  :"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10","r11","r12","r13","r14","r15","r16","r17","r19","r24","r25","r26","r27","r28","r29"
		  );

#endif  //end of CONTIKI_TARGET_MICAZ

#ifdef CONTIKI_TARGET_SKY  //should implement in assembly
    //width=1
    //accumulator, r9~r11
    //i, r12
    //j, r13
    //address of b, for m_a, r14
    //address of c, for m_b / tmp, r15

    asm volatile ("clr r9 \n\t" //init accumulator
		  "clr r10 \n\t"
		  "clr r11 \n\t"
		  "clr r12 \n\t" //i=0
		  "dec %2 \n\t"  //digits--
		  "SQR_LOOP1: clr r13 \n\t" //j=0
		  "mov r12, r14 \n\t"
		  "inc r14 \n\t"
		  "add r14, r14 \n\t"
		  "add %1, r14 \n\t"
		  "SQR_LOOP2: mov r12, r15 \n\t"
		  "sub r13, r15 \n\t"  //i-j
		  "cmp r13, r15 \n\t"  //i-j ? j
		  "jeq EQ1 \n\t"  //i-j == j
		  "jge GR1 \n\t"  //i-j > j
		  "jmp SQR_LOOP2_EXIT \n\t"  //i-j < j
		  "GR1: sub #2, r14 \n\t"  //address is ready for m_a
		  "mov r13, r15 \n\t"
		  "add r15, r15 \n\t"
		  "add %1, r15 \n\t"  //address is ready for m_b
		  "mov @r14, &0x130 \n\t"  //load m_a
		  "mov @r15, &0x138 \n\t"  //load m_b, get mul result
		  "add &0x13A, r9 \n\t" //add to accumulator
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "add &0x13A, r9 \n\t" //add to accumulator
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "inc r13 \n\t"
		  "jmp SQR_LOOP2 \n\t"
		  "EQ1: sub #2, r14 \n\t"  //address is ready for m_a
		  "mov r13, r15 \n\t"
		  "add r15, r15 \n\t"
		  "add %1, r15 \n\t"  //address is ready for m_b
		  "mov @r14, &0x130 \n\t"  //load m_a
		  "mov @r15, &0x138 \n\t"  //load m_b, get mul result
		  "add &0x13A, r9 \n\t" //add to accumulator
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "SQR_LOOP2_EXIT: mov r9, 0(%0) \n\t"
		  "add #2, %0 \n\t"
		  "mov r10, r9 \n\t"
		  "mov r11, r10 \n\t"
		  "clr r11 \n\t"
		  "cmp %2, r12 \n\t"
		  "jeq SQR_LOOP1_EXIT \n\t"
		  "inc r12 \n\t"  //i++
		  "jmp SQR_LOOP1 \n\t"
		  "SQR_LOOP1_EXIT: inc r12 \n\t" //i=[n/d]
		  "SQR_LOOP3: mov r12, r13 \n\t"
		  "sub %2, r13 \n\t" //j=i-[n/d]
		  "mov %2, r14 \n\t"
		  "inc r14 \n\t"
		  "add r14, r14 \n\t"
		  "add %1, r14 \n\t"
		  "SQR_LOOP4: mov r12, r15 \n\t"
		  "sub r13, r15 \n\t"  //i-j
		  "cmp r13, r15 \n\t"  //i-j ? j
		  "jeq EQ2 \n\t"  //i-j == j
		  "jge GR2 \n\t"  //i-j > j
		  "jmp SQR_LOOP4_EXIT \n\t"  //i-j < j
		  "GR2: sub #2, r14 \n\t"  //address is ready for m_a
		  "mov r13, r15 \n\t"
		  "add r15, r15 \n\t"
		  "add %1, r15 \n\t"  //address is ready for m_b
		  "mov @r14, &0x130 \n\t"  //load m_a
		  "mov @r15, &0x138 \n\t"  //load m_b, get mul result
		  "add &0x13A, r9 \n\t" //add to accumulator
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "add &0x13A, r9 \n\t" //add to accumulator
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "inc r13 \n\t"
		  "jmp SQR_LOOP4 \n\t"
		  "EQ2: sub #2, r14 \n\t"  //address is ready for m_a
		  "mov r13, r15 \n\t"
		  "add r15, r15 \n\t"
		  "add %1, r15 \n\t"  //address is ready for m_b
		  "mov @r14, &0x130 \n\t"  //load m_a
		  "mov @r15, &0x138 \n\t"  //load m_b, get mul result
		  "add &0x13A, r9 \n\t" //add to accumulator
		  "addc &0x13C, r10 \n\t"
		  "addc #0, r11 \n\t"
		  "SQR_LOOP4_EXIT: mov r9, 0(%0) \n\t"
		  "add #2, %0 \n\t"
		  "mov r10, r9 \n\t"
		  "mov r11, r10 \n\t"
		  "mov #0, r11 \n\t"
		  "mov %2, r15 \n\t"
		  "add r15, r15 \n\t"
		  "cmp r12, r15 \n\t"
		  "jeq SQR_LOOP3_EXIT \n\t"
		  "inc r12 \n\t"
		  "jmp SQR_LOOP3 \n\t"
		  "SQR_LOOP3_EXIT:mov r9, 0(%0) \n\t"
		  :
		  :"r"(a), "r"(b), "r"(digits)
		  :"r9", "r10", "r11", "r12", "r13", "r14", "r15"
		  );

    /*

    NN_DIGIT t[2*MAX_NN_DIGITS];
    NN_UINT bDigits, i;

    NN_AssignZero (t, 2 * digits);
  
    bDigits = NN_Digits (b, digits);
    
    for (i = 0; i < bDigits; i++)
      t[i+bDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], b, bDigits);
  
    NN_Assign (a, t, 2 * digits);
    */
#endif  //end of CONTIKI_TARGET_SKY

#ifdef CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki

    //r0~r2
    //r3 b
    //r4 i
    //r5 j
    //r6 address for b
    //r7 tmp, address for c
    //r8 tmp, c

    asm volatile ("STMFD r13!, {r0 - r8} \n\t"
		  "MOV r0, #0 \n\t"  //init accumulator
		  "MOV r1, #0 \n\t"
		  "MOV r2, #0 \n\t"  //end of init
		  "SUB %2, %2, #1 \n\t"  //[n/d]-1
		  "MOV r4, #0 \n\t"  //i
		  "SQR_LOOP1: MOV r5, #0 \n\t"  //j=0
		  "ADD r7, r4, #1 \n\t"  //(i+1)
		  "ADD r6, %1, r7, lsl #2 \n\t"  //address is ready for b
		  "SQR_LOOP2: SUB r8, r4, r5 \n\t"
		  "CMP r8, r5 \n\t"   //compare i-j and j
		  "BHI HI1 \n\t"
		  "BEQ EQ1 \n\t"
		  "B SQR_LOOP2_EXIT \n\t"
		  "HI1: LDR r3, [r6, #-4]! \n\t"  //load b0~b(d-1)
		  "ADD r7, %1, r5, lsl #2 \n\t"  //compute address for c
		  "LDR r8, [r7] \n\t"  //load c[j*d]  //compute address for c
		  "UMULL r7, r8, r3, r8 \n\t"  //t=0
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "ADD r5, r5, #1 \n\t" //j++
		  "B SQR_LOOP2 \n\t"
		  "EQ1: LDR r3, [r6, #-4]! \n\t"  //load b0~b(d-1)
		  "ADD r7, %1, r5, lsl #2 \n\t"  //compute address for c
		  "LDR r8, [r7] \n\t"  //load c[j*d]  //compute address for c
		  "UMULL r7, r8, r3, r8 \n\t"  //t=0
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "B SQR_LOOP2_EXIT \n\t"
		  "SQR_LOOP2_EXIT: STR r0, [%0], #4 \n\t"  //a[i*d] = r0
		  "MOV r0, r1 \n\t"
		  "MOV r1, r2 \n\t"
		  "MOV r2, #0 \n\t"
		  "CMP r4, %2 \n\t"
		  "BEQ SQR_LOOP1_EXIT \n\t"
		  "ADD r4, r4, #1 \n\t" //i++
		  "B SQR_LOOP1 \n\t"
		  "SQR_LOOP1_EXIT: ADD r4, r4, #1 \n\t"  //i = [n/d]
		  "SQR_LOOP3: SUB r5, r4, %2 \n\t"  //j=i-[n/d]
		  "ADD r7, %2, #1 \n\t"
		  "ADD r6, %1, r7, lsl #2 \n\t"  //address for b is ready
		  "SQR_LOOP4: SUB r8, r4, r5 \n\t"
		  "CMP r8, r5 \n\t"   //compare i-j and j
		  "BHI HI2 \n\t"
		  "BEQ EQ2 \n\t"
		  "B SQR_LOOP4_EXIT \n\t"
		  "HI2: LDR r3, [r6, #-4]! \n\t"  //load b0~b[d-1]
		  "ADD r7, %1, r5, lsl #2 \n\t"  //compute address for c
		  "LDR r8, [r7] \n\t"		  
		  "UMULL r7, r8, r3, r8 \n\t"  //t=0
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "ADD r5, r5, #1 \n\t"  //j++
		  "B SQR_LOOP4 \n\t"
		  "EQ2: LDR r3, [r6, #-4]! \n\t"  //load b0~b[d-1]
		  "ADD r7, %1, r5, lsl #2 \n\t"  //compute address for c
		  "LDR r8, [r7] \n\t"		  
		  "UMULL r7, r8, r3, r8 \n\t"  //t=0
		  "ADDS r0, r0, r7 \n\t"
		  "ADCS r1, r1, r8 \n\t"
		  "ADCCS r2, r2, #0 \n\t"
		  "B SQR_LOOP4_EXIT \n\t"
		  "SQR_LOOP4_EXIT: STR r0, [%0], #4 \n\t"  //a[i*d] = r0
		  "MOV r0, r1 \n\t"
		  "MOV r1, r2 \n\t"
		  "MOV r2, #0 \n\t"
		  "CMP r4, %2, lsl #1 \n\t"
		  "BEQ SQR_LOOP3_EXIT \n\t"
		  "ADD r4, r4, #1 \n\t"
		  "B SQR_LOOP3 \n\t"
		  "SQR_LOOP3_EXIT: STR r0, [%0], #4 \n\t"
		  "LDMFD r13!, {r0 - r8} \n\t"
		  :
		  :"r"(a), "r"(b), "r"(digits)
		  :"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8"
		  );

    
#endif

#else  //no inline assembly

    NN_DIGIT t[2*MAX_NN_DIGITS];
    NN_UINT bDigits, i;

    NN_AssignZero (t, 2 * digits);
  
    bDigits = NN_Digits (b, digits);
    
    for (i = 0; i < bDigits; i++)
      t[i+bDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], b, bDigits);
  
    NN_Assign (a, t, 2 * digits);

#endif
  }



  /* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.
     a, b can be same
     Lengths: a[digits], b[digits].
     Requires c < NN_DIGIT_BITS.
   */
  NN_DIGIT NN_LShift (NN_DIGIT *a, NN_DIGIT *b, NN_UINT c, NN_UINT digits)
  {
    NN_DIGIT bi, carry;
    NN_UINT i, t;
  
    if (c >= NN_DIGIT_BITS)
      return (0);
  
    t = NN_DIGIT_BITS - c;

    carry = 0;

    for (i = 0; i < digits; i++) {
      bi = b[i];
      a[i] = (bi << c) | carry;
      carry = c ? (bi >> t) : 0;
    }
  
    return (carry);
  }

  /* Computes a = b div 2^c (i.e., shifts right c bits), returning carry.
     a, b can be same
     Lengths: a[digits], b[digits].
     Requires: c < NN_DIGIT_BITS.
   */
  NN_DIGIT NN_RShift (NN_DIGIT *a, NN_DIGIT *b, NN_UINT c, NN_UINT digits)
  {
    NN_DIGIT bi, carry;
    int i;
    NN_UINT t;
  
    if (c >= NN_DIGIT_BITS)
      return (0);
  
    t = NN_DIGIT_BITS - c;

    carry = 0;

    for (i = digits - 1; i >= 0; i--) {
      bi = b[i];
      a[i] = (bi >> c) | carry;
      carry = c ? (bi << t) : 0;
    }
  
    return (carry);
  }

  /* Computes a = c div d and b = c mod d.
     a, c, d can be same
     b, c, d can be same
     Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
     Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS,
             dDigits < MAX_NN_DIGITS.
   */
  void NN_Div (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT cDigits, NN_DIGIT *d, NN_UINT dDigits)
  {
    NN_DIGIT ai, cc[2*MAX_NN_DIGITS+2], dd[MAX_NN_DIGITS+1], t;

    int i;
    int ddDigits, shift;
  
    ddDigits = NN_Digits (d, dDigits);
    if (ddDigits == 0)
      return;
  
    /* Normalize operands.
     */
    shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
    NN_AssignZero (cc, ddDigits);
    cc[cDigits] = NN_LShift (cc, c, shift, cDigits);
    NN_LShift (dd, d, shift, ddDigits);
    t = dd[ddDigits-1];

    if (a != NULL)
    	NN_AssignZero (a, cDigits);

    for (i = cDigits-ddDigits; i >= 0; i--) {
      /* Underestimate quotient digit and subtract.
       */
      if (t == MAX_NN_DIGIT)
        ai = cc[i+ddDigits];
      else
        NN_DigitDiv (&ai, &cc[i+ddDigits-1], t + 1);
      cc[i+ddDigits] -= NN_SubDigitMult (&cc[i], &cc[i], ai, dd, ddDigits);

      /* Correct estimate.
       */
      while (cc[i+ddDigits] || (NN_Cmp (&cc[i], dd, ddDigits) >= 0)) {
        ai++;
        cc[i+ddDigits] -= NN_Sub (&cc[i], &cc[i], dd, ddDigits);
      }
      if (a != NULL)
        a[i] = ai;
    }  
    /* Restore result.
     */
    NN_AssignZero (b, dDigits);
    NN_RShift (b, cc, shift, ddDigits);
  }

  /* Computes a = b mod c.

     Lengths: a[cDigits], b[bDigits], c[cDigits].
     Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS, cDigits < MAX_NN_DIGITS.
   */
  void NN_Mod (NN_DIGIT *a, NN_DIGIT *b, NN_UINT bDigits, NN_DIGIT *c, NN_UINT cDigits)
  {  
#ifdef BARRETT_REDUCTION
    NNModBarrett(a, b, bDigits, c, cDigits);
#else
    NN_Div (NULL, a, b, bDigits, c, cDigits);
#endif
  }

  /* Computes a = b * c mod d.
     a, b, c can be same
     Lengths: a[digits], b[digits], c[digits], d[digits].
     Assumes d > 0, digits < MAX_NN_DIGITS.
   */
  void NN_ModMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d, NN_UINT digits)
  {
#ifdef BARRETT_REDUCTION

    NN_DIGIT q2[2*MAX_NN_DIGITS+8], x[2*MAX_NN_DIGITS+8], r2[2*MAX_NN_DIGITS+8], m[MAX_NN_DIGITS+4];

    if(digits > 4)
      memset(x+2*digits-8, 0, (2*MAX_NN_DIGITS-2*digits+16)*NN_DIGIT_LEN);
    else
      memset(x, 0, (2*MAX_NN_DIGITS+8)*NN_DIGIT_LEN);

    //b*c
    NN_Mult(x, b, c, digits);
    memcpy(m, d, digits*NN_DIGIT_LEN);
    memset(m+digits, 0, (MAX_NN_DIGITS+4-digits)*NN_DIGIT_LEN);

    //q_2=q_1*mu
    NN_Mult(q2, x+pBarrett->km-1, pBarrett->mu, pBarrett->mu_len);
    //q_3*m
    NN_Mult(r2, q2+pBarrett->km+1, m, pBarrett->mu_len);
    memset(r2+pBarrett->km+1, 0, (2*MAX_NN_DIGITS+8-pBarrett->km-1)*NN_DIGIT_LEN);
    memset(x+pBarrett->km+1, 0, (2*MAX_NN_DIGITS+8-pBarrett->km-1)*NN_DIGIT_LEN);

    if (NN_Cmp(x, r2, pBarrett->km+1) < 0)
      x[pBarrett->km+1] = 1;
    NN_Sub(x, x, r2, pBarrett->km+2);

    while(NN_Cmp(x, m, digits) >= 0)
      NN_Sub(x, x, m, digits);

    memcpy(a, x, digits*NN_DIGIT_LEN);

    /*
    NN_DIGIT q2[2*MAX_NN_DIGITS+8], x[2*MAX_NN_DIGITS+8], r2[2*MAX_NN_DIGITS+8], m[MAX_NN_DIGITS+4];
    //int i; //for debug

    //b*c
    NN_Mult(x, b, c, digits);
    memset(x+2*digits, 0, (2*MAX_NN_DIGITS+8-2*digits)*NN_DIGIT_LEN);

    memcpy(m, d, digits*NN_DIGIT_LEN);
    memset(m+digits, 0, (MAX_NN_DIGITS+4-digits)*NN_DIGIT_LEN);

    //q_2=q_1*mu
    NN_Mult(q2, x+km-1, mu, mu_len);

    //q_3*m
    NN_Mult(r2, q2+km+1, m, mu_len);
    memset(r2+km+1, 0, (2*MAX_NN_DIGITS+8-km-1)*NN_DIGIT_LEN);

    memset(x+km+1, 0, (2*MAX_NN_DIGITS+8-km-1)*NN_DIGIT_LEN);
    if (NN_Cmp(x, r2, km+1) < 0)
      x[km+1] = 1;
    NN_Sub(x, x, r2, km+2);
    while(NN_Cmp(x, m, digits) >= 0)
      NN_Sub(x, x, m, digits);
    memcpy(a, x, digits*NN_DIGIT_LEN);
*/
#else
    NN_DIGIT t[2*MAX_NN_DIGITS];
    
    //memset(t, 0, 2*MAX_NN_DIGITS*NN_DIGIT_LEN);
    t[2*MAX_NN_DIGITS-1]=0;
    t[2*MAX_NN_DIGITS-2]=0;
    NN_Mult (t, b, c, digits);
    NN_Mod (a, t, 2 * digits, d, digits);
#endif
  }

  /* Computes a = b^c mod d.

     Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
     Assumes d > 0, cDigits > 0, dDigits < MAX_NN_DIGITS.
   */
  void NN_ModExp (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT cDigits, NN_DIGIT *d, NN_UINT dDigits)
  {
    NN_DIGIT bPower[3][MAX_NN_DIGITS], ci, t[MAX_NN_DIGITS];
    int i;
    uint8_t ciBits, j, s;

    /* Store b, b^2 mod d, and b^3 mod d.
     */
    NN_Assign (bPower[0], b, dDigits);
    NN_ModMult (bPower[1], bPower[0], b, d, dDigits);
    NN_ModMult (bPower[2], bPower[1], b, d, dDigits);
  
    NN_ASSIGN_DIGIT (t, 1, dDigits);

    cDigits = NN_Digits (c, cDigits);
    for (i = cDigits - 1; i >= 0; i--) {
      ci = c[i];
      ciBits = NN_DIGIT_BITS;
      
      /* Scan past leading zero bits of most significant digit.
       */
      if (i == (int)(cDigits - 1)) {
        while (! DIGIT_2MSB (ci)) {
          ci <<= 2;
          ciBits -= 2;
        }
      }

      for (j = 0; j < ciBits; j += 2, ci <<= 2) {
        /* Compute t = t^4 * b^s mod d, where s = two MSB's of ci.
         */
        NN_ModMult (t, t, t, d, dDigits);
        NN_ModMult (t, t, t, d, dDigits);
        if ((s = DIGIT_2MSB (ci)) != 0)
          NN_ModMult (t, t, bPower[s-1], d, dDigits);
      }
    }
  
    NN_Assign (a, t, dDigits);
  }

  /* Compute a = 1/b mod c, assuming inverse exists.
     a, b, c can be same
     Lengths: a[digits], b[digits], c[digits].
     Assumes gcd (b, c) = 1, digits < MAX_NN_DIGITS.
   */
  void NN_ModInv (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits)
  {
    NN_DIGIT q[MAX_NN_DIGITS], t1[MAX_NN_DIGITS], t3[MAX_NN_DIGITS],
      u1[MAX_NN_DIGITS], u3[MAX_NN_DIGITS], v1[MAX_NN_DIGITS],
      v3[MAX_NN_DIGITS], w[2*MAX_NN_DIGITS];
    int u1Sign;
    /* Apply extended Euclidean algorithm, modified to avoid negative
       numbers.
     */
    NN_ASSIGN_DIGIT (u1, 1, digits);
    NN_AssignZero (v1, digits);
    NN_Assign (u3, b, digits);
    NN_Assign (v3, c, digits);
    u1Sign = 1;

    while (! NN_Zero (v3, digits)) {
      NN_Div (q, t3, u3, digits, v3, digits);
      NN_Mult (w, q, v1, digits);
      NN_Add (t1, u1, w, digits);
      NN_Assign (u1, v1, digits);
      NN_Assign (v1, t1, digits);
      NN_Assign (u3, v3, digits);
      NN_Assign (v3, t3, digits);
      u1Sign = -u1Sign;
    }
  
    /* Negate result if sign is negative.
      */
    if (u1Sign < 0)
      NN_Sub (a, c, u1, digits);
    else
      NN_Assign (a, u1, digits);

  }

  /*
   * a= b/c mod d
   * algorithm in "From Euclid's GCD to Montgomery Multiplication to the Great Divide"
   * 
   *
   */
  void NN_ModDivOpt (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d, NN_UINT digits)
  {
    NN_DIGIT A[MAX_NN_DIGITS], B[MAX_NN_DIGITS], U[MAX_NN_DIGITS], V[MAX_NN_DIGITS];
    int tmp_even;

    NN_Assign(A, c, digits);
    NN_Assign(B, d, digits);
    NN_Assign(U, b, digits);
    NN_AssignZero(V, digits);
    
    while ((tmp_even = NN_Cmp(A, B, digits)) != 0){
  	  watchdog_periodic();

	  if (NN_EVEN(A, digits)){
	NN_RShift(A, A, 1, digits);
	if (NN_EVEN(U, digits)){
	  NN_RShift(U, U, 1, digits);
	}else{
	  NN_Add(U, U, d, digits);
	  NN_RShift(U, U, 1, digits);
	}
      }else if (NN_EVEN(B, digits)){
	NN_RShift(B, B, 1, digits);
	if (NN_EVEN(V, digits)){
	  NN_RShift(V, V, 1, digits);
	}else{
	  NN_Add(V, V, d, digits);
	  NN_RShift(V, V, 1, digits);
	}
      }else if (tmp_even > 0){
	NN_Sub(A, A, B, digits);
	NN_RShift(A, A, 1, digits);
	if (NN_Cmp(U, V, digits) < 0){
	  NN_Add(U, U, d, digits);
	}
	NN_Sub(U, U, V, digits);
	if (NN_EVEN(U, digits)){
	  NN_RShift(U, U, 1, digits);
	}else{
	  NN_Add(U, U, d, digits);
	  NN_RShift(U, U, 1, digits);
	}
      }else{
	NN_Sub(B, B, A, digits);
	NN_RShift(B, B, 1, digits);
	if (NN_Cmp(V, U, digits) < 0){
	  NN_Add(V, V, d, digits);
	}
	NN_Sub(V, V, U, digits);
	if (NN_EVEN(V, digits)){
	  NN_RShift(V, V, 1, digits);
	}else{
	  NN_Add(V, V, d, digits);
	  NN_RShift(V, V, 1, digits);
	}
      }
    }

    NN_Assign(a, U, digits);
  }

  /* Computes a = gcd(b, c).
     a, b, c can be same
     Lengths: a[digits], b[digits], c[digits].
     Assumes b > c, digits < MAX_NN_DIGITS.
   */
  void NN_Gcd (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits)
  {
    NN_DIGIT t[MAX_NN_DIGITS], u[MAX_NN_DIGITS], v[MAX_NN_DIGITS];

    NN_Assign (u, b, digits);
    NN_Assign (v, c, digits);

    while (! NN_Zero (v, digits)) {
      NN_Mod (t, u, digits, v, digits);
      NN_Assign (u, v, digits);
      NN_Assign (v, t, digits);
    }

    NN_Assign (a, u, digits);

  }


  /* Returns sign of a - b.

     Lengths: a[digits], b[digits].
   */
  int NN_Cmp (NN_DIGIT *a, NN_DIGIT *b, NN_UINT digits)
  {
    int i;
  
    for (i = digits - 1; i >= 0; i--) { 
      if (a[i] > b[i])
        return (1);
      /* else added by Panos Kampankis*/
      else if (a[i] < b[i])
        return (-1); 
    }

    return (0);
  }

  /* Returns nonzero iff a is zero.

     Lengths: a[digits].
   */
  int NN_Zero (NN_DIGIT *a, NN_UINT digits)
  {
    NN_UINT i;
  
    for (i = 0; i < digits; i++)
      if (a[i])
        return (0);
    
    return (1);
  }

  /* Returns the significant length of a in bits.

     Lengths: a[digits].
   */
  unsigned int NN_Bits (NN_DIGIT *a, NN_UINT digits)
  {
    if ((digits = NN_Digits (a, digits)) == 0)
      return (0);
  
    return ((digits - 1) * NN_DIGIT_BITS + NN_DigitBits (a[digits-1]));
  }

  /* Returns the significant length of a in digits.

    Lengths: a[digits].
   */
  unsigned int NN_Digits (NN_DIGIT *a, NN_UINT digits)
  {
    int i;
  
    for (i = digits - 1; i >= 0; i--)
      if (a[i])
        break;

    return (i + 1);
  }

  /* Computes a = b + c*d, where c is a digit. Returns carry.
     a, b, c can be same
     Lengths: a[digits], b[digits], d[digits].
   */
  static NN_DIGIT NN_AddDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, NN_UINT digits)
  {
#ifdef INLINE_ASM
#ifdef CONTIKI_TARGET_MICAZ
    NN_DIGIT carry;
    unsigned int i;
    if (c == 0)
      return (0);
    carry = 0;
    for (i = 0; i < digits; i++) {
      asm volatile ("push r1\n\t"
                    "add %2, %1\n\t"
                    "mov %0, %2\n\t"
                    "ldi %1, 0\n\t"
                    "rol %1\n\t"
                    "mul %3, %4\n\t"
                    "add %0, r0\n\t"
                    "adc %1, r1\n\t"
                    "pop r1\n\t"
                    :"+r"(a[i]), "+r"(carry)
                    :"r"(b[i]), "r"(c), "r"(d[i])
                    );
    }
    return (carry);
#endif
#ifdef CONTIKI_TARGET_SKY
    NN_DIGIT carry;
    unsigned int i;
    if (c == 0)
      return (0);
    carry = 0;
    for (i = 0; i < digits; i++) {
      asm volatile ("add %1, %2 \n\t"
                    "mov %2, %0 \n\t"
                    "mov #0, %1 \n\t"
                    "rlc %1 \n\t"
		    "mov %3, &0x130 \n\t"
		    "mov %4, &0x138 \n\t"
                    "add &0x13a, %0 \n\t"
                    "addc &0x13c, %1 \n\t"
                    :"+r"(a[i]), "+r"(carry)
                    :"r"(b[i]), "r"(c), "r"(d[i])
                    );
    }
    return (carry);
#endif

#ifdef CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki
    NN_DIGIT carry;
    unsigned int i;
    NN_DOUBLE_DIGIT t;

    //Should copy b to a
    if (c == 0)
      return (0);
    carry = 0;
    for (i = 0; i < digits; i++) {
      t = NN_DigitMult (c, d[i]);
      if ((a[i] = b[i] + carry) < carry)
        carry = 1;
      else
        carry = 0;
      if ((a[i] += (t & MAX_NN_DIGIT)) < (t & MAX_NN_DIGIT))
        carry++;
      carry += (NN_DIGIT)(t >> NN_DIGIT_BITS);
    }
    return (carry);
#endif

#else

    NN_DIGIT carry;
    unsigned int i;
    NN_DOUBLE_DIGIT t;

    //Should copy b to a
    if (c == 0)
      return (0);
    carry = 0;
    for (i = 0; i < digits; i++) {
      t = NN_DigitMult (c, d[i]);
      if ((a[i] = b[i] + carry) < carry)
        carry = 1;
      else
        carry = 0;
      if ((a[i] += (t & MAX_NN_DIGIT)) < (t & MAX_NN_DIGIT))
        carry++;
      carry += (NN_DIGIT)(t >> NN_DIGIT_BITS);
    }
    return (carry);
#endif

  }

  /* Computes a = b - c*d, where c is a digit. Returns borrow.
     a, b, d can be same
     Lengths: a[digits], b[digits], d[digits].
   */
  static NN_DIGIT NN_SubDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, NN_UINT digits)
  {
#ifdef INLINE_ASM
#ifdef CONTIKI_TARGET_MICAZ
    NN_DIGIT borrow;
    unsigned int i;

    if (c == 0)
      return (0);
    borrow = 0;
    for (i = 0; i < digits; i++) {
      asm volatile ("push r1\n\t"
                    "sub %2, %1\n\t"
                    "mov %0, %2\n\t"
                    "ldi %1, 0\n\t"
                    "rol %1\n\t"
                    "mul %3, %4\n\t"
                    "sub %0, r0\n\t"
                    "adc %1, r1\n\t"
                    "pop r1\n\t"
                    :"+r"(a[i]), "+r"(borrow)
                    :"r"(b[i]), "r"(c), "r"(d[i])
                    );
    }
    return (borrow);
#endif
#ifdef CONTIKI_TARGET_SKY
    NN_DIGIT borrow;
    unsigned int i;

    if (c == 0)
      return (0);
    borrow = 0;
    for (i = 0; i < digits; i++) {
      asm volatile ("sub %1, %2 \n\t"
                    "mov %2, %0 \n\t"
                    "mov #0, %1 \n\t"
                    "rlc %1 \n\t"
		    "xor #0x0001, %1 \n\t"
		    "mov %3, &0x130 \n\t"
		    "mov %4, &0x138 \n\t"
                    "sub &0x13A, %0 \n\t"
		    "xor #0x0001, r2 \n\t"
                    "addc &0x13C, %1 \n\t"
		    :"+r"(a[i]), "+r"(borrow)
                    :"r"(b[i]), "r"(c), "r"(d[i])
                    );
    }
    return (borrow);
#endif

#ifdef CONTIKI_TARGET_IMOTE2 // Not yet supported by Contiki
    NN_DIGIT borrow;
    unsigned int i;
    NN_DOUBLE_DIGIT t;

    if (c == 0)
      return (0);
    borrow = 0;
    for (i = 0; i < digits; i++) {
      t = NN_DigitMult (c, d[i]);
      if ((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
        borrow = 1;
      else
        borrow = 0;
      if ((a[i] -= (t & MAX_NN_DIGIT)) > (MAX_NN_DIGIT - (t & MAX_NN_DIGIT)))
        borrow++;
      borrow += (NN_DIGIT)(t >> NN_DIGIT_BITS);
    }
    return (borrow);
#endif

#else
    NN_DIGIT borrow;
    unsigned int i;
    NN_DOUBLE_DIGIT t;

    if (c == 0)
      return (0);
    borrow = 0;
    for (i = 0; i < digits; i++) {
      t = NN_DigitMult (c, d[i]);
      if ((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
        borrow = 1;
      else
        borrow = 0;
      if ((a[i] -= (t & MAX_NN_DIGIT)) > (MAX_NN_DIGIT - (t & MAX_NN_DIGIT)))
        borrow++;
      borrow += (NN_DIGIT)(t >> NN_DIGIT_BITS);
    }
    return (borrow);    
#endif

  }

  /* Returns the significant length of a in bits, where a is a digit.
   */
  static unsigned int NN_DigitBits (NN_DIGIT a)
  {
    unsigned int i;
  
    for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
      if (a == 0)
        break;
    
    return (i);
  }

 // Returns the e{k-1}(b) Lucas function
 void NN_Lucas (NN_DIGIT * a,NN_DIGIT * b, NN_DIGIT * k,NN_DIGIT * d,NN_UINT digits) {
   int i;
   NN_DIGIT two[NUMWORDS], k2[NUMWORDS];
   NN_DIGIT alpha[NUMWORDS];

   memset(two, 0, NUMWORDS*NN_DIGIT_LEN);
   two[0]= 0x1;
   NN_Sub(k2,k,two,digits);
   two[0]= 0x2;

   memset(alpha, 0, NUMWORDS*NN_DIGIT_LEN);
   alpha[0]=0x2;
   NN_Assign(a,b,digits); //beta=b
   
   i = (NNBits(k2,NUMWORDS))-1;
   		
   while (i>-1) {
	if (NNTestBit(k2,i)) {
		NN_ModMult(alpha,alpha,a,d,digits); 
		NNModSub(alpha,alpha,b,d,digits); //a=a*beta-P
		NNModSqr(a, a, d, digits);
		NNModSub(a,a,two,d,digits); //beta=beta^2-2
	}
	
	else {
		NN_ModMult(a,alpha,a,d,digits);
		NNModSub(a,a,b,d,digits); //beta=a*beta-P
		NNModSqr(alpha, alpha, d, digits);
		NNModSub(alpha,alpha,two,d,digits); //a=a^2-2
	}
	i--;
   }
 }

/* CONVERSIONS */
void NNDecode(NN_DIGIT * a, NN_UINT digits, unsigned char * b, NN_UINT len)
  {
    NN_Decode (a, digits, b, len);
  }
   

void NNEncode(unsigned char * a, NN_UINT digits, NN_DIGIT * b, NN_UINT len)
  {
    NN_Encode (a, digits, b, len);
  }

/* ASSIGNMENTS */
  void NNAssign(NN_DIGIT * a, NN_DIGIT * b, NN_UINT digits)
  {
    NN_Assign (a, b, digits);
  }

  void NNAssignDigit(NN_DIGIT * a, NN_DIGIT b, NN_UINT digits)
  {
    NN_AssignZero (a, digits);
    a[0] = b;
  }
   
  void NNAssignZero(NN_DIGIT * a, NN_UINT digits)
  {
    NN_AssignZero (a, digits);
  }
  
  void NNAssign2Exp(NN_DIGIT * a, NN_UINT2 b, NN_UINT digits)
  {
    NN_Assign2Exp (a, b, digits);
  }
     
  /* ARITHMETIC OPERATIONS */
  //Computes a = b + c.
  NN_DIGIT NNAdd(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits)
  {
    return NN_Add (a, b, c, digits);
  }
  
  //Computes a = b - c.
  NN_DIGIT NNSub(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits)
  {
    return NN_Sub (a, b, c, digits);
  }
  	  
  //Computes a = b * c.
  void NNMult(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits)
  {
    NN_Mult (a, b, c, digits);
  }
  
  //Computes a = c div d and b = c mod d.
  void NNDiv(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT cDigits, NN_DIGIT * d, NN_UINT dDigits)
  {
    return NN_Div (a, b, c, cDigits, d, dDigits);
  }
  
  //Computes a = b * 2^c.
  NN_DIGIT NNLShift(NN_DIGIT * a, NN_DIGIT * b, NN_UINT c, NN_UINT digits)
  {
    return NN_LShift (a, b, c, digits);
  }
  
  //Computes a = b / 2^c.
  NN_DIGIT NNRShift(NN_DIGIT * a, NN_DIGIT * b, NN_UINT c, NN_UINT digits)
  {
    return NN_RShift (a, b, c, digits);
  }
  
  NN_DIGIT NNAddDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, NN_UINT digits)
  {
    return NN_AddDigitMult(a, b, c, d, digits);
  }
  
/* NUMBER THEORY */
  //Computes a = b mod c.
  void NNMod(NN_DIGIT * a, NN_DIGIT * b, NN_UINT bDigits, NN_DIGIT * c, NN_UINT cDigits)
  {
    NN_Mod (a, b, bDigits, c, cDigits);
  }



  //init \mu , b = wordsize, from Handbook of Applied Cryptography Algorithm 14.42
  void NNModBarrettInit(NN_DIGIT *c, NN_UINT cDigits, Barrett *pbuf)
  {
    NN_DIGIT tmp[2*MAX_NN_DIGITS+1], residue[MAX_NN_DIGITS];

    pBarrett = pbuf;
    pBarrett->km = NN_Digits(c, cDigits);

    memset(tmp, 0, (2*MAX_NN_DIGITS+1)*NN_DIGIT_LEN);
    memset(pBarrett->mu, 0, (2*MAX_NN_DIGITS+1)*NN_DIGIT_LEN);
    tmp[2*pBarrett->km] = 1;

    NN_Div(pBarrett->mu, residue, tmp, 2*pBarrett->km+1, c, cDigits);
    pBarrett->mu_len = NN_Digits(pBarrett->mu, 2*MAX_NN_DIGITS+1);

#ifdef HYBRID_MULT
#ifdef CONTIKI_TARGET_MICAZ
#ifdef HYBRID_MUL_WIDTH4
    while ((pBarrett->mu_len % 4) != 0)
      pBarrett->mu_len++;
#else
    while ((pBarrett->mu_len % 5) != 0)
      pBarrett->mu_len++;
#endif
#endif
#endif


  }

  //set the buffer for barrett reduction
  void NNBarrettSetBuf(Barrett *pbuf){
    pBarrett = pbuf;
  }



  //Barrett modular reduction, from Handbook of Applied Cryptography Algorithm 14.42
  //compute a = b mod c
  //cDigits must be smaller than MAX_NN_DIGITS
  void NNModBarrett(NN_DIGIT *a, NN_DIGIT *b, NN_UINT bDigits, NN_DIGIT *c, NN_UINT cDigits)
  {

    NN_DIGIT q2[2*MAX_NN_DIGITS+6], q1[MAX_NN_DIGITS+4], r2[2*MAX_NN_DIGITS+8], r1[MAX_NN_DIGITS+4], m[MAX_NN_DIGITS+4];
    //int i; //for debug
    //trace(DBG_USR1, "enter into barrett mod\n\r");
    memset(q2, 0, (2*MAX_NN_DIGITS+6)*NN_DIGIT_LEN);
    memcpy(q1, b+pBarrett->km-1, (bDigits-pBarrett->km+1)*NN_DIGIT_LEN);
    memset(q1+bDigits-pBarrett->km+1, 0, MAX_NN_DIGITS+4-bDigits+pBarrett->km-1);
    memcpy(r1, b, (pBarrett->km+1)*NN_DIGIT_LEN);
    memset(r1+pBarrett->km+1, 0, (MAX_NN_DIGITS+4-pBarrett->km-1)*NN_DIGIT_LEN);
    memcpy(m, c, cDigits*NN_DIGIT_LEN);
    memset(m+cDigits, 0, MAX_NN_DIGITS+4-cDigits);

    //q_2=q_1*mu
    NN_Mult(q2, q1, pBarrett->mu, pBarrett->mu_len);
    //q_3*m
    NN_Mult(r2, q2+pBarrett->km+1, m, pBarrett->mu_len);
    memset(r2+pBarrett->km+1, 0, (2*MAX_NN_DIGITS+8-pBarrett->km-1)*NN_DIGIT_LEN);
    if (NN_Cmp(r1, r2, pBarrett->km+1) < 0)
      r1[pBarrett->km+1] = 1;
    NN_Sub(r1, r1, r2, pBarrett->km+2);

    while(NN_Cmp(r1, m, cDigits) >= 0)
      NN_Sub(r1, r1, m, cDigits);

    memcpy(a, r1, cDigits*NN_DIGIT_LEN);

  }

  // b = b mod c
  void NNModSmall(NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits)
  {
	  while (NN_Cmp(b, c, digits) >= 0) {
		  NN_Sub(b, b, c, digits);
	  }
  }
   
  //Computes a = (b + c) mod d.
  //a, b, c can be same
  //Assumption: b,c is in [0, d)
  void NNModAdd(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits)
  {
    NN_DIGIT tmp[MAX_NN_DIGITS];
    NN_DIGIT carry;
    
    carry = NN_Add(tmp, b, c, digits);
    if (carry)
      NN_Sub(a, tmp, d, digits);
    else if (NN_Cmp(tmp, d, digits) >= 0)
      NN_Sub(a, tmp, d, digits);
    else
      NN_Assign(a, tmp, digits);
    
  }
   
  //Computes a = (b - c) mod d.
  //Assume b and c are all smaller than d
  //always return positive value
  void NNModSub(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits)
  {
    NN_DIGIT tmp[MAX_NN_DIGITS];
    NN_DIGIT borrow;
    
    borrow = NN_Sub(tmp, b, c, digits);
    if (borrow) 
      NN_Add(a, tmp, d, digits); 
    else 
      NN_Assign(a, tmp, digits);
    
  }
  
  //Computes a = -b mod c
  void NNModNeg(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits) {
     NNModSub(a,c,b,c,digits);
  }
   
  //Computes a = b * c mod d.
  void NNModMult(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits)
  {
    NN_ModMult (a, b, c, d, digits);
  }
  
  //Computes a = (b / c) mod d
  void NNModDiv(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits)
  {
    /*
#if defined(INLINE_ASM) && defined(CONTIKI_TARGET_MICAZ)
   //with assembly code for multiplication it will be faster the traditional way
     NN_DIGIT t1[NUMWORDS];
     NN_ModInv (t1, c, d, digits);
     NN_ModMult (a, b, t1, d, digits);
     #else*/
     NN_ModDivOpt(a,b,c,d,digits);
     //#endif
  }
  
  //Computes a = (b / c) mod d using the opt way
  void NNModDivOpt(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_UINT digits)
  {
     NN_ModDivOpt(a,b,c,d,digits);
  }
   
  //Computes a = b * c mod d, d is generalized mersenne prime, d = 2^KEYBITS - omega
  void NNModMultOpt(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_DIGIT * d, NN_DIGIT * omega, NN_UINT digits)
  {
#ifdef CURVE_OPT
    NN_DIGIT t1[2*MAX_NN_DIGITS];
    NN_DIGIT t2[2*MAX_NN_DIGITS];
    NN_DIGIT *pt1;
    NN_UINT len_t2, len_t1;
    
    //memset(t1, 0, 2*MAX_NN_DIGITS*NN_DIGIT_LEN);
    //memset(t2+KEYDIGITS*NN_DIGIT_LEN, 0, (2*MAX_NN_DIGITS-KEYDIGITS)*NN_DIGIT_LEN);
    t1[2*MAX_NN_DIGITS-1]=0;
    t1[2*MAX_NN_DIGITS-2]=0;
    t2[2*MAX_NN_DIGITS-1]=0;
    t2[2*MAX_NN_DIGITS-2]=0;

    NN_Mult (t1, b, c, KEYDIGITS);
     
    pt1 = &(t1[KEYDIGITS]);
    len_t2 = 2*KEYDIGITS;
     
    //the "Curve-Specific Optimizations" algorithm in "Comparing Elliptic Curve Cryptography and RSA on 8-bit CPUs"
    while (!NN_Zero(pt1, KEYDIGITS)){
      memset(t2, 0, len_t2*NN_DIGIT_LEN);
      len_t2 -= KEYDIGITS;
      len_t1 = len_t2;
      len_t2 = omega_mul(t2, pt1, omega, len_t2);
      memset(pt1, 0, len_t1*NN_DIGIT_LEN);
      NN_Add(t1, t2, t1, MAX(KEYDIGITS,len_t2)+1);
    }

    while (NN_Cmp(t1, d, digits) > 0) 
      NN_Sub(t1, t1, d, digits);      
    NN_Assign (a, t1, digits);
#else
    NN_ModMult (a, b, c, d, digits);    
#endif
  }
  
  //Computes a = b^2 mod d, The Standard Squaring Algorithm in "High-Speed RSA Implementation"
  void NNModSqr(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * d, NN_UINT digits)
  {
#ifdef BARRETT_REDUCTION

    NN_DIGIT q2[2*MAX_NN_DIGITS+8], x[2*MAX_NN_DIGITS+8], r2[2*MAX_NN_DIGITS+8], m[MAX_NN_DIGITS+4];
    //int i; //for debug

    //b^2
    NN_Sqr(x, b, digits);
    memset(x+2*digits, 0, (2*MAX_NN_DIGITS+8-2*digits)*NN_DIGIT_LEN);

    memcpy(m, d, digits*NN_DIGIT_LEN);
    memset(m+digits, 0, (MAX_NN_DIGITS+4-digits)*NN_DIGIT_LEN);

    //q_2=q_1*mu
    NN_Mult(q2, x+pBarrett->km-1, pBarrett->mu, pBarrett->mu_len);

    //q_3*m
    NN_Mult(r2, q2+pBarrett->km+1, m, pBarrett->mu_len);
    memset(r2+pBarrett->km+1, 0, (2*MAX_NN_DIGITS+8-pBarrett->km-1)*NN_DIGIT_LEN);

    memset(x+pBarrett->km+1, 0, (2*MAX_NN_DIGITS+8-pBarrett->km-1)*NN_DIGIT_LEN);
    if (NN_Cmp(x, r2, pBarrett->km+1) < 0)
      x[pBarrett->km+1] = 1;
    NN_Sub(x, x, r2, pBarrett->km+2);
    while(NN_Cmp(x, m, digits) >= 0)
      NN_Sub(x, x, m, digits);
    memcpy(a, x, digits*NN_DIGIT_LEN);

#else
    NN_DIGIT t[2*MAX_NN_DIGITS];
    
    NN_Sqr (t, b, digits);
    NN_Mod (a, t, 2 * digits, d, digits);
#endif
  }
   
  void NNModSqrOpt(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * d, NN_DIGIT * omega, NN_UINT digits)
  {
#ifdef CURVE_OPT
    NN_DIGIT t1[2*MAX_NN_DIGITS];
    NN_DIGIT t2[2*MAX_NN_DIGITS];
    NN_DIGIT *pt1;
    NN_UINT len_t1, len_t2;

    t1[2*MAX_NN_DIGITS-1]=0;
    t1[2*MAX_NN_DIGITS-2]=0;
    t2[2*MAX_NN_DIGITS-1]=0;
    t2[2*MAX_NN_DIGITS-2]=0;

    NN_Sqr(t1, b, KEYDIGITS);
     
    pt1 = &(t1[KEYDIGITS]);
    len_t2 = 2*KEYDIGITS;
    //the "Curve-Specific Optimizations" algorithm in "Comparing Elliptic Curve Cryptography and RSA on 8-bit CPUs"
    while (!NN_Zero(pt1, KEYDIGITS)){
      memset(t2, 0, len_t2*NN_DIGIT_LEN);
      len_t2 -= KEYDIGITS;
      len_t1 = len_t2;
      len_t2 = omega_mul(t2, pt1, omega, len_t2);
      memset(pt1, 0, len_t1*NN_DIGIT_LEN);
      NN_Add(t1, t2, t1, MAX(KEYDIGITS,len_t2)+1);
    }
     
    while (NN_Cmp(t1, d, digits) > 0)
      NN_Sub(t1, t1, d, digits);
    NN_Assign (a, t1, digits);
#else
    NN_DIGIT t[2*MAX_NN_DIGITS];
    int bDigits;

    bDigits = NN_Digits (b, digits);
    if (bDigits < MAX_NN_DIGITS)
      NN_AssignZero(t+2*bDigits, 2*MAX_NN_DIGITS-2*bDigits);
    NN_Sqr (t, b, digits);
    NN_Mod (a, t, 2 * digits, d, digits);
#endif
  }
   
  //Computes a = b^c mod d.
  void NNModExp(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT cDigits, NN_DIGIT * d, NN_UINT dDigits)
  {
    NN_ModExp (a, b, c, cDigits, d, dDigits);
  }
  //Computes a = 1/b mod c.
  void NNModInv(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits)
  {
#ifndef MODINVOPT
    NN_ModInv (a, b, c, digits);
#else
	NN_DIGIT y[MAX_NN_DIGITS];
    NN_ASSIGN_DIGIT(y, 1, digits);
    NN_ModDivOpt(a, y, b, c, digits);
#endif
    
  }

  //Computes a = gcd (b, c).
  void NNGcd(NN_DIGIT * a, NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits)
  {
    NN_Gcd (a, b, c, digits);
  }

  //P1363 A.2.4
  void Lucas_Sequence(NN_DIGIT *V0, NN_DIGIT *Q0, NN_DIGIT *P, NN_DIGIT *Q, NN_DIGIT *k, NN_DIGIT *p, NN_DIGIT *omega){
    NN_DIGIT v0[NUMWORDS], v1[NUMWORDS], q0[NUMWORDS], q1[NUMWORDS];
    NN_DIGIT tmp[NUMWORDS];
    int r;

    memset(v0, 0, NUMWORDS*NN_DIGIT_LEN);
    v0[0] = 2;
    memcpy(v1, P, NUMWORDS*NN_DIGIT_LEN);
    memset(q0, 0, NUMWORDS*NN_DIGIT_LEN);
    q0[0] = 1;
    memset(q1, 0, NUMWORDS*NN_DIGIT_LEN);
    q1[1] = 1;

    r = NN_Bits(k, NUMWORDS) - 1;

    while(r >= 0){
      NNModMultOpt(q0, q0, q1, p, omega, NUMWORDS);
      if(b_testbit(k, r)){
	NNModMultOpt(q1, q0, Q, p, omega, NUMWORDS);
	NNModMultOpt(tmp, P, q0, p, omega, NUMWORDS);
	NNModMultOpt(v0, v0, v1, p, omega, NUMWORDS);
	NNModSub(v0, v0, tmp, p, NUMWORDS);
	NNModSqrOpt(v1, v1, p, omega, NUMWORDS);
	NNModSub(v1, v1, q1, p, NUMWORDS);
	NNModSub(v1, v1, q1, p, NUMWORDS);
      }else{
	memcpy(q1, q0, NUMWORDS*NN_DIGIT_LEN);
	NNModMultOpt(v1, v1, v0, p, omega, NUMWORDS);
	NNModMultOpt(tmp, P, q0, p, omega, NUMWORDS);
	NNModSub(v1, v1, tmp, p, NUMWORDS);
	NNModSqrOpt(v0, v0, p, omega, NUMWORDS);
	NNModSub(v0, v0, q0, p, NUMWORDS);
	NNModSub(v0, v0, q0, p, NUMWORDS);
      }
    }
    memcpy(V0, v0, NUMWORDS*NN_DIGIT_LEN);
    memcpy(Q0, q0, NUMWORDS*NN_DIGIT_LEN);

  }

  //a = b^(1/2) mod c, from P1363 A.2.5
  int NNModSqrRootOpt(NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT digits, NN_DIGIT *omega){
    NN_DIGIT m[NUMWORDS];
    NN_DIGIT k[NUMWORDS], residue[NUMWORDS];
    NN_DIGIT gam[NUMWORDS], i[NUMWORDS], twog[NUMWORDS];

    //case 1.
    memset(m, 0, NUMWORDS*NN_DIGIT_LEN);
    m[0] = 4;
    NN_Div(k, residue, c, NUMWORDS, m, NUMWORDS);
    if (residue[0] == 3){
      m[0] = 1;
      NN_Add(k, k, m, NUMWORDS);
      NN_ModExp(a, b, k, NUMWORDS, c, digits);
      return 1;
    }

    //case 2.
    m[0] = 8;
    NN_Div(k, residue, c, NUMWORDS, m, NUMWORDS);
    if(residue[0] == 5){
      NN_LShift(twog, b, 1, NUMWORDS);
      if(NN_Cmp(twog, c, NUMWORDS) >=0)
	NN_Sub(twog, twog, c, NUMWORDS);
      NN_ModExp(gam, twog, k, NUMWORDS, c, digits);
#ifdef CURVE_OPT
      NNModSqrOpt(i, gam, c, omega, digits);
      NNModMultOpt(i, i, twog, c, omega, digits);
      m[0] = 1;
      NNModSub(i, i, m, c, digits);
      NNModMultOpt(a, b, gam, c, omega, digits);
      NNModMultOpt(a, a, i, c, omega, digits);
#else
      NNModSqr(i, gam, c, digits);
      NNModMult(i, i, twog, c, digits);
      m[0] = 1;
      NNModSub(i, i, m, c, digits);
      NNModMult(a, b, gam, c, digits);
      NNModMult(a, a, i, c, digits);
#endif
      return 1;
    }

    //case 3.
    m[0] = 8;
    NN_Div(k, residue, c, NUMWORDS, m, NUMWORDS);
    if(residue[0] == 1){
      //Q = b
      
      //0<P<p
      memset(i, 0, NUMWORDS*NN_DIGIT_LEN);
      i[0] = 1;
      m[0] = 1;
      //k = (p+1)/2
      memcpy(k, c, NUMWORDS*NN_DIGIT_LEN);
      NN_Add(k, k, m, NUMWORDS);
      NN_RShift(k, k, 1, NUMWORDS);

      while (NN_Cmp(i, c, NUMWORDS) < 0){
	//compute V
	Lucas_Sequence(gam, twog, i, b, k, c, omega); //V = gam, Q = twog
	//z = V/2 mod p
	if ((gam[0] & 0x1) == 1)
	  NN_Add(gam, gam, c, NUMWORDS);	  
	NN_RShift(gam, gam, 1, NUMWORDS);
	//check whether z^2 mod p = b
	NNModSqrOpt(residue, gam, c, omega, NUMWORDS);
	if(NN_Cmp(residue, b, NUMWORDS) == 0){
	  memcpy(a, gam, NUMWORDS*NN_DIGIT_LEN);
	  return 1;
	}
	if(NN_Cmp(twog, m, NUMWORDS) > 0){
	  NN_Add(twog, twog, m, NUMWORDS);
	  if(NN_Cmp(twog, c, NUMWORDS) < 0)
	    return -1;
	}
	NN_Add(i, i, m, NUMWORDS);
      }
    }

    return -1;


  }


  
/* OTHER OPERATIONS */
  //Returns sign of a - b.
  int NNCmp(NN_DIGIT * a, NN_DIGIT * b, NN_UINT digits)
  {
    return NN_Cmp (a, b, digits);
  }

  //Returns 1 iff a = 0.
  int NNZero(NN_DIGIT * a, NN_UINT digits)
  {
    return NN_Zero (a, digits);
  }

  //returns 1 iff a = 1
  int NNOne(NN_DIGIT * a, NN_UINT digits)
  {
    uint8_t i;
    
    for (i = 1; i < digits; i++)
      if (a[i])
        return FALSE;
    if (a[0] == 1)
      return TRUE;
    
    return FALSE;
  }

  //Returns significant length of a in bits.
  unsigned int NNBits(NN_DIGIT * a, NN_UINT digits)
  {
    return NN_Bits (a, digits);
  }

  // test whether the ith bit in a is one
  NN_DIGIT NNTestBit(NN_DIGIT * a, int16_t i)
  {  
      return (b_testbit(a,i));
  }

  //Returns significant length of a in digits.
  unsigned int NNDigits(NN_DIGIT * a, NN_UINT digits)
  {
    return NN_Digits (a, digits);
  }

  //whether a is even or not
  bool NNEven(NN_DIGIT * a, NN_UINT digits)
  {
    return (((digits) == 0) || ! (a[0] & 1));
  }

  //whether a equals to b or not
  bool NNEqual(NN_DIGIT * a, NN_DIGIT * b, NN_UINT digits)
  {
    return (! NN_Cmp (a, b, digits));
  }

  // Return the lucas exponentatiation for the Tate Pairing lucas(2*b,k)/2
  bool NNLucExp(NN_DIGIT * a,NN_DIGIT * b, NN_DIGIT * k,NN_DIGIT * inv2,NN_DIGIT * d,NN_UINT digits) {

    NN_DIGIT  temp1[NUMWORDS], temp2[NUMWORDS];
/*    NN_DIGIT two[NUMWORDS];
    memset(two, 0, NUMWORDS*NN_DIGIT_LEN);
    two[0] = 0x02;

   NN_ModMult(temp1,two,b,d,digits); // 2b */
   // 2b it is faster with shif than the above line
   NN_LShift(temp1, b, 1, digits);
   if(NN_Cmp(temp1, d, digits) >= 0)
     NN_Sub(temp1, temp1, d, digits);

   NN_Lucas(temp2,temp1,k,d,digits); // lucas(2b,k)
   NNModMult(a,temp2,inv2,d,digits); // lucas(2b,k)*2^(-1)

   return TRUE;
  }

/* --- Extension TUD -------------------------------------------------------- */

/**
 * @brief Generate random, non-zero b, with b = b mod c.
 * code taken from ECC_gen_private_key
 */
void NNModRandom (NN_DIGIT * b, NN_DIGIT * c, NN_UINT digits) {
	NN_UINT order_digit_len, order_bit_len;
    bool done = FALSE;
    uint8_t ri;
    NN_DIGIT digit_mask;
	
    order_bit_len = NNBits(c, NUMWORDS);
	order_digit_len = NNDigits(c, NUMWORDS);

    while(!done){
		watchdog_periodic();
		
		for (ri=0; ri < order_digit_len; ri++){
#ifdef THIRTYTWO_BIT_PROCESSOR
			b[ri] = ((uint32_t)rand() << 16)^((uint32_t)rand());
#else
			b[ri] = (NN_DIGIT)rand();
#endif
		}

		for (ri=order_digit_len; ri<NUMWORDS; ri++){
			b[ri] = 0;
		}

		if (order_bit_len % NN_DIGIT_BITS != 0){
			digit_mask = MAX_NN_DIGIT >> (NN_DIGIT_BITS - order_bit_len % NN_DIGIT_BITS);
			b[order_digit_len - 1] = b[order_digit_len - 1] & digit_mask;
		}
		NNModSmall(b, c, digits);
		
		if (NNZero(b, digits) != 1)
			done = TRUE;
		watchdog_periodic();
	}
}

/*
 * @brief Assign 1. a = 1.
 */
void NNAssignOne(NN_DIGIT * a, NN_UINT digits)
{
    uint8_t i;
    
    for (i = 1; i < digits; i++)
		a[i] = 0;
    
	a[0] = 1;
}

/*
 * @brief Computes same as NNModMult, but is more efficient with small modulo values.
 * Computes a = b * c mod d.
 *   a, b, c can be same
 *   Lengths: a[cDigits], b[cDigits], c[cDigits], d[dDigits].
 *   Assumes d > 0, cDigits < MAX_NN_DIGITS, dDigits < cDigits.
 * /
// TODO: does not work, but code works if copied in places ?!!
void NNModMultVar (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_UINT cDigits, NN_DIGIT *d, NN_UINT dDigits)
{
	NN_DIGIT tmp[2*cDigits];

	NNMult(tmp, b, c, cDigits);		
	NNDiv(NULL, tmp, tmp, 2*cDigits, d, dDigits);
	NNAssignZero(a, NUMWORDS);
	NNAssign(a, tmp, dDigits);

}*/


