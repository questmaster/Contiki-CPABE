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
 * Interface for Tate Pairing curve parameters
 *
 * Author: Panos Kampanakis
 * Date: 02/04/2007
 */

#ifndef _TPCURVEPARAM_H_
#define _TPCURVEPARAM_H_

#include <ECC.h>


extern void get_param(Params *para);

//get the parameters of specific curve for Tate Pairing
extern void get_TP_param(TPParams *tppara);



  // Quick and dirty, trail & error...
void get_param(Params *para) {
	TPParams tpp;
	int i;
	
	get_TP_param(&tpp);
	
	// set ECC_params
	para->E.a_minus3 = tpp.E.a_minus3;
	para->E.a_zero = tpp.E.a_zero;
	para->E.a_one = tpp.E.a_one;
	
	for (i = 0; i < NUMWORDS; i++) {
		para->p[i] = tpp.p[i];
		para->omega[i] = 0;
		para->E.a[i] = tpp.E.a[i];
		para->E.b[i] = tpp.E.b[i];
		para->G.x[i] = tpp.P.x[i];
		para->G.y[i] = tpp.P.y[i];
		para->r[i] = tpp.m[i];
	}
	
}

#endif
