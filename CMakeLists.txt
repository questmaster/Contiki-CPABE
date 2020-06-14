cmake_minimum_required(VERSION 3.16)
project(Contiki_CPABE)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(linux)
add_subdirectory(apps/ecc)
add_subdirectory(apps/cp-abe)

add_executable(test_CPABE
        linux/testCPABE.c
        )

target_compile_definitions(test_CPABE PUBLIC
        CPABE_SETUP CPABE_KEYGEN CPABE_ENCRYPTION CPABE_DECRYPTION
        CPABE_SMALL_SET
        TARGET_LINUX32
        CPABE192K2
        TPSSC
        )

target_link_libraries(test_CPABE ECC CPABE LinSupport)

