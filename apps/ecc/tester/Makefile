CONTIKI_PROJECT = testECC
all: $(CONTIKI_PROJECT)

APPS = ecc

## choose curve parameter
#Cparam = SECP128R1
#Cparam = SECP128R2
#Cparam = SECP160K1
#Cparam = SECP160R1
#Cparam = SECP160R2
#Cparam = SECP192K1
#Cparam = SECP192R1
Cparam = SS192K2
#Cparam = SS192K2S
#Cparam = SS512K2
#Cparam = SS512K2S

# use test vector for secp160r1 to show the correctness of TinyECC
#Cparam = TEST_VECTOR
#CFLAGS+=-DCODE_SIZE

# use EC algorithms
USE_ECDH = TRUE
USE_ECIES = TRUE
USE_ECDSA = TRUE
#USE_PAIRING = TRUE

## choose different optimization techniques
## NN
CFLAGS+=-DBARRETT_REDUCTION		# barrett reduction
CFLAGS+=-DHYBRID_MULT			# hybrid multipliation
CFLAGS+=-DHYBRID_SQR			# hybrid squre
#CFLAGS+=-DCURVE_OPT			# optimization for secg curve 
								# (Not possible for TP -> non-functioning omega optimization)
## ECC
CFLAGS+=-DPROJECTIVE			# projective coordinate
CFLAGS+=-DSLIDING_WIN			# sliding window method, window size is defined in ECC.h
## ECDSA
CFLAGS+=-DSHAMIR_TRICK			# shamir trick, windows size is defined in ECDSAM.nc

## activate 4/8MHz configuration (compatible to TinyOS freq's)
CFLAGS += -DMSP430_4MHZ_ENABLE
#CFLAGS += -DMSP430_8MHZ_ENABLE

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
