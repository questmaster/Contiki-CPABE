
set(ECC_COMPILE_DEFINITIONS
        ## choose different optimization techniques
        ## NN
        BARRETT_REDUCTION		# barrett reduction
        #HYBRID_MULT			# hybrid multipliation
        #HYBRID_SQR			# hybrid square
        NO_ASM
        #CURVE_OPT			# optimization for secg curve
        # (Not possible for TP -> non-functioning omega optimization)
        ## ECC
        PROJECTIVE			# projective coordinate
        SLIDING_WIN			# sliding window method, window size is defined in ECC.h
        ## ECDSA
        SHAMIR_TRICK			# shamir trick, windows size is defined in ECDSA.c
        ## TP
        #FIXED_P				# precompute intermediate nodes, slopes for a fixed P
        #PROJECTIVE_M			# ???

        # to be removed
        TARGET_LINUX32 CPABE192K2 TPSSC
)

