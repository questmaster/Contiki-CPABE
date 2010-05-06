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
 * $Id: ECDSA.nc,v 1.2 2007/09/12 18:17:06 aliu3 Exp $
 * interface for ECDSA
 *
 * Author: An Liu
 * Date: 09/15/2005
 */

#ifndef _ECDSA_H_
#define _ECDSA_H_

#include <ECC.h>
#include <NN.h>

	//init the ECDSA, pKey is public key used to verify the signature
	//we assume that the node has already know the public key when deployed
	extern bool ECDSA_init(Point * pKey) __attribute__ ((noinline));
	
	//do signature on msg, (r,s) is the signature, d is the private key
	extern void ECDSA_sign(uint8_t *msg, uint8_t len, NN_DIGIT *r, NN_DIGIT *s, NN_DIGIT *d) __attribute__ ((noinline));
	
	//verify the signature (r,s), Q is the public key. return 1 if passed.
	extern uint8_t ECDSA_verify(uint8_t *msg, uint8_t len, NN_DIGIT *r, NN_DIGIT *s, Point *Q) __attribute__ ((noinline));


#endif

