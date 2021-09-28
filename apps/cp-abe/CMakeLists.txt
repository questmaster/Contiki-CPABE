cmake_minimum_required(VERSION 3.16)

project(libCPABE
        VERSION ${PROJECT_VERSION}
        LANGUAGES C
        )

add_library (CPABE STATIC EXCLUDE_FROM_ALL
        cp-abe.c
#        curves/cpabe168k2.c
        curves/cpabe192k2.c
#        curves/cpabe224k2.c
#        curves/cpabe256k2.c
#        curves/cpabe384k2.c
#        curves/cpabe512k2.c
        qsort.c
        )

target_link_libraries (CPABE
        PUBLIC ECC
        )

target_include_directories (CPABE
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_definitions(CPABE PRIVATE
        CPABE_SETUP
        CPABE_KEYGEN
        CPABE_ENCRYPTION
        CPABE_DECRYPTION
        )

setup_module(CPABE)

add_subdirectory(test)
