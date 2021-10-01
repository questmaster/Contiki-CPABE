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

include(ecc_config.cmake)
target_compile_definitions(ECC PUBLIC ${ECC_COMPILE_DEFINITIONS})

setup_module(ECC)

add_subdirectory(test)

