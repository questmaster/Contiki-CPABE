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
 * Interface NN2
 * Provide the functions of big Natural Complex Numbers.
 * 
 * Author: Panos Kampanakis
 * Date: 02/05/2007
 */

includes NN2;

interface NN2 {
	  //multiplies two complex numbers
  	  command void ModMult(NN2_NUMBER * a, NN2_NUMBER * b, NN2_NUMBER * c, NN_DIGIT * d, NN_UINT digits);
	  //squre of a complex number
  	  command void ModSqr(NN2_NUMBER * a, NN2_NUMBER * b, NN_DIGIT * d, NN_UINT digits);
  	  //assign a = b
  	  command void Assign(NN2_NUMBER * a,NN2_NUMBER * b,NN_UINT digits);
}
