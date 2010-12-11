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
 * interface for TP
 *
 * Author: Panos Kampanakis
 * Date: 02/04/2005
 */

#ifndef _TP_H_
#define _TP_H_

#include <ECC.h>
#include <NN2.h>
#include <TPCurveParam.h>

  //initialize the Tate Pairing between the private key of the curve P and the key given Q
//  extern void TP_init(Point P, Point Q);
  //Miller's algorithm
//  extern bool TP_Miller(NN2_NUMBER *ef, Point P);
  //final exponentiation in Miller's algorithm
//  extern void TP_final_expon(NN_DIGIT *r,NN2_NUMBER *ef);
  //Tate Pairing Computation
//  extern void TP_computeTP(NN_DIGIT *res, Point P);

//*** Don't use the above! ***//

  // One Step Tate pairing
  extern void TP_TatePairing(NN2_NUMBER *res, Point P, Point Q);

  extern TPParams *TP_getTPparams();
#endif
