cmake_minimum_required(VERSION 3.16)

project(libECC
        VERSION ${PROJECT_VERSION}
        LANGUAGES C
        )

add_library(ECC
        ECC.c
#        ECDH.c
#        ECDSA.c
#        ECIES.c
        NN.c
        NN2.c
#        curves/secp128r1.c
#        curves/secp128r2.c
#        curves/secp160k1.c
#        curves/secp160r1.c
#        curves/secp160r2.c
#        curves/secp192k1.c
#        curves/secp192r1.c
#        curves/ss192k2.c
#        curves/ss192k2s.c
#        curves/ss512k2.c
#        curves/ss512k2s.c
        TP.c
        TPCurveParam.c
        sha1.c
        )

target_link_libraries (ECC
        PUBLIC LinSupport
        )

target_include_directories (ECC
        PUBLIC
#        ${CMAKE_CURRENT_SOURCE_DIR}/include
#        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_definitions(ECC PUBLIC
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

setup_module(ECC)

#add_subdirectory(test)
