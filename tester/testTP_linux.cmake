add_executable(test_TP
        testTP_linux.c
        ../apps/ecc/curves/ss192k2s.c
        )

target_link_libraries(test_TP ECC LinSupport)

#target_compile_definitions(test_TP PUBLIC
#        CPABE_SETUP CPABE_KEYGEN CPABE_ENCRYPTION CPABE_DECRYPTION
#        CPABE_SMALL_SET
#        TARGET_LINUX32
#        CPABE192K2
#        TPSSC
#        )

setup_module(test_TP)

add_test(IntegrationTP test_TP)