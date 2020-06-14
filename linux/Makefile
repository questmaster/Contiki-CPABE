# Config flag for nn.c
CFLAGS += -DTARGET_LINUX32
#CFLAGS += -DCONTIKI_TARGET_SKY


## choose curve parameter
#Cparam = SECP128R1
#Cparam = SECP128R2
#Cparam = SECP160K1
#Cparam = SECP160R1
#Cparam = SECP160R2
#Cparam = SECP192K1
#Cparam = SECP192R1

## possible for TP/CPABE
#Cparam = SS192K2
#Cparam = SS192K2S
#Cparam = SS512K2
#Cparam = SS512K2S
Cparam = CPABE192K2

# use test vector for secp160r1 to show the correctness of TinyECC (NOT TP or CPABE!)
#Cparam = TEST_VECTOR
#CFLAGS+=-DCODE_SIZE

# use of EC algorithms
#USE_ECDH = TRUE
#USE_ECIES = TRUE
#USE_ECDSA = TRUE
#USE_PAIRING = TRUE
USE_CPABE = TRUE

# en-/disable different parts of CPABE code and DEBUG messages
#CFLAGS+=-DCPABE_DEBUG
CFLAGS+=-DCPABE_SETUP
CFLAGS+=-DCPABE_KEYGEN
CFLAGS+=-DCPABE_ENCRYPTION
CFLAGS+=-DCPABE_DECRYPTION

# CPABE policy/property sets
CFLAGS+=-DCPABE_SMALL_SET
#CFLAGS+=-DCPABE_MEDIUM_SET
#CFLAGS+=-DCPABE_LARGE_SET


## choose different optimization techniques
## NN
CFLAGS+=-DBARRETT_REDUCTION		# barrett reduction
CFLAGS+=-DHYBRID_MULT			# hybrid multipliation
CFLAGS+=-DHYBRID_SQR			# hybrid square
CFLAGS+=-DNO_ASM
#CFLAGS+=-DCURVE_OPT			# optimization for secg curve 
								# (Not possible for TP -> non-functioning omega optimization)
## ECC
CFLAGS+=-DPROJECTIVE			# projective coordinate
CFLAGS+=-DSLIDING_WIN			# sliding window method, window size is defined in ECC.h
## ECDSA
CFLAGS+=-DSHAMIR_TRICK			# shamir trick, windows size is defined in ECDSA.c
## TP
#CFLAGS+=-DFIXED_P				# precompute intermediate nodes, slopes for a fixed P
#CFLAGS+=-DPROJECTIVE_M			# ???


include ../Makefile.ecc


################################################################################
## Start build process ## Don't change anything except you know what you do! ###
################################################################################

CC=arm-linux-gcc
TARGET=testCPABE
OTHERDIR=../

SOURCE = $(TARGET).c
SOURCE += $(ecc_src)
SOURCE += list.c watchdog.c

## End sources definition
CFLAGS += -I./  
CFLAGS += -I$(OTHERDIR)
## end more includes

VPATH=$(OTHERDIR)
OBJ=$(join $(addsuffix obj/, $(dir $(SOURCE))), $(notdir $(SOURCE:.c=.o))) 

## Fix dependency destination to be .dep relative to the src dir
DEPENDS=$(join $(addsuffix .dep/, $(dir $(SOURCE))), $(notdir $(SOURCE:.c=.d)))

## Default rule executed
all: $(TARGET)
		@true

## Clean Rule
clean:
		@-rm -f $(TARGET) $(OBJ) $(DEPENDS)


## Rule for making the actual target
$(TARGET): $(OBJ)
		@echo "============="
		@echo "Linking the target $@"
		@echo "============="
		@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
		@echo -- Link finished --

## Generic compilation rule
%.o : %.c
		@mkdir -p $(dir $@)
		@echo "============="
		@echo "Compiling $<"
		@$(CC) $(CFLAGS) -c $< -o $@


## Rules for object files from cpp files
## Object file for each file is put in obj directory
## one level up from the actual source directory.
obj/%.o : %.c
		@mkdir -p $(dir $@)
		@echo "============="
		@echo "Compiling $<"
		@$(CC) $(CFLAGS) -c $< -o $@

# Rule for "other directory"  You will need one per "other" dir
$(OTHERDIR)/obj/%.o : %.c
		@mkdir -p $(dir $@)
		@echo "============="
		@echo "Compiling $<"
		@$(CC) $(CFLAGS) -c $< -o $@

## Make dependancy rules
.dep/%.d: %.c
		@mkdir -p $(dir $@)
		@echo "============="
		@echo Building dependencies file for $*.o
		@$(SHELL) -ec '$(CC) -M $(CFLAGS) $(INCLUDE) $< | sed "s^$*.o^obj/$*.o^" > $@'

## Dependency rule for "other" directory
$(OTHERDIR)/.dep/%.d: %.c
		@mkdir -p $(dir $@)
		@echo "============="
		@echo Building dependencies file for $*.o
		@$(SHELL) -ec '$(CC) -M $(CFLAGS) $(INCLUDE) $< | sed "s^$*.o^$(OTHERDIR)/obj/$*.o^" > $@'

## Include the dependency files
-include $(DEPENDS)
