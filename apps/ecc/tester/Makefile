CONTIKI_PROJECT = testECC
all: $(CONTIKI_PROJECT)

APPS = ecc


## choose curve parameter

## only ECC !
#Cparam = SECP128R1
#Cparam = SECP128R2
#Cparam = SECP160K1
#Cparam = SECP160R1
#Cparam = SECP160R2
#Cparam = SECP192K1
#Cparam = SECP192R1

## ECC / TP
#Cparam = SS192K2
#Cparam = SS192K2S
##Cparam = SS512K2
#Cparam = SS512K2S
Cparam = CPABE192K2

# use test vector for secp160r1 to show the correctness of TinyECC (NOT TP or CPABE!)
#Cparam = TEST_VECTOR
#CFLAGS+=-DCODE_SIZE

# use EC algorithms
#USE_ECDH = TRUE
#USE_ECIES = TRUE
#USE_ECDSA = TRUE
#USE_PAIRING = TRUE
USE_CPABE = TRUE

# en-/disable different parts of testCPABE code and DEBUG messages
#CFLAGS+=-DCPABE_DEBUG
CFLAGS+=-DCPABE_SETUP
CFLAGS+=-DCPABE_KEYGEN
CFLAGS+=-DCPABE_ENCRYPTION
CFLAGS+=-DCPABE_DECRYPTION

CFLAGS+=-DCPABE_SMALL_SET
#CFLAGS+=-DCPABE_MEDIUM_SET
#CFLAGS+=-DCPABE_LARGE_SET

## choose different optimization techniques
## NN
CFLAGS+=-DBARRETT_REDUCTION		# barrett reduction
#CFLAGS+=-DHYBRID_MULT			# hybrid multipliation
#CFLAGS+=-DHYBRID_SQR			# hybrid square
#CFLAGS+=-DCURVE_OPT			# optimization for secg curve 
					# (Not possible for TP -> non-functioning omega optimization)
CFLAGS+=-DNO_ASM				# disable assembler speed-up for add, mult, exp,... 
								# (NOT needed for MicaZ, Tmote/Telosb, Imote2/Stargate)
## ECC
CFLAGS+=-DPROJECTIVE			# projective coordinate
CFLAGS+=-DSLIDING_WIN			# sliding window method, window size is defined in ECC.h
## ECDSA
CFLAGS+=-DSHAMIR_TRICK			# shamir trick, windows size is defined in ECDSA.c
## TP
#CFLAGS+=-DFIXED_P			# precompute intermediate nodes, slopes for a fixed P
#CFLAGS+=-DPROJECTIVE_M			# ???

## activate 4/8MHz configuration for Tmote Sky (compatible to TinyOS freq's)
#CFLAGS += -DMSP430_4MHZ_ENABLE		# Only Tmote on Contiki 2.4
#CFLAGS += -DMSP430_8MHZ_ENABLE		# Only Tmote on Contiki 2.4
#CFLAGS += -DMSP430_16MHZ_ENABLE	# Only Z1 on Contiki 2.5


#TARGETDIRS += apps
APPDIRS += ../../

ifndef CONTIKI
	CONTIKI = ../../../contiki
endif
include $(CONTIKI)/Makefile.include

