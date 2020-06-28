add_definitions(
        ## choose different optimization techniques
        ## NN
        -DBARRETT_REDUCTION		# barrett reduction
        #-DHYBRID_MULT			# hybrid multipliation
        #-DHYBRID_SQR			# hybrid square
        -DNO_ASM
        #-DCURVE_OPT			# optimization for secg curve
        # (Not possible for TP -> non-functioning omega optimization)
        ## ECC
        -DPROJECTIVE			# projective coordinate
        -DSLIDING_WIN			# sliding window method, window size is defined in ECC.h
        ## ECDSA
        -DSHAMIR_TRICK			# shamir trick, windows size is defined in ECDSA.c
        ## TP
        #-DFIXED_P				# precompute intermediate nodes, slopes for a fixed P
        #-DPROJECTIVE_M			# ???

        # to be removed
        -DTARGET_LINUX32 -DCPABE192K2 -DTPSSC
        )
