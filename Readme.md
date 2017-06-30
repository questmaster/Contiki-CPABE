# CP-ABE Implementation for Contiki

This repository contains an implementation of CP-ABE for the Contiki 2.4/2.5 operating system including support files.
Performane measurements on several platforms are documented in this thesis: http://tuprints.ulb.tu-darmstadt.de/5249/

## Repository Contents

- Contiki module 'ecc'
    - TinyECC ported to Contiki
    - TinyECC extended for CP-ABE
    - CP-ABE algorithm
    - Test code for Contiki and Linux
    - Supporting tools
    
## Usage

- copy 'ecc' module to Contiki module directory
- configure test code in 'ecc/tester/Makefile'
- build test code in 'ecc/tester' as always in Contiki

## Build parameters

See 'apps/ecc/tester/Makefile'

## Known issues

- Code accepts only some curves (cause currently unknown)

## Questions

For support use the Github-Issues page: https://github.com/questmaster/Contiki-CPABE/issues

## Legal stuff

- Original TinyECC License

All new code in this distribution is Copyright 2010 by North Carolina State University. All rights reserved. Redistribution and use in source and binary forms are permitted provided that this entire copyright notice is duplicated in all such copies, and that any documentation, announcements, and other materials related to such distribution and use acknowledge that the software was developed at North Carolina State University, Raleigh, NC. No charge may be made for copies, derivations, or distributions of this material without the express written consent of the copyright holder. Neither the name of the University nor the name of the author may be used to endorse or promote products derived from this material without specific prior written permission.

IN NO EVENT SHALL THE NORTH CAROLINA STATE UNIVERSITY BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE NORTH CAROLINA STATE UNIVERSITY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE NORTH CAROLINA STATE UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."

