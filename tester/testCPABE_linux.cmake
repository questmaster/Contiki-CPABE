add_executable(test_CPABE
        testCPABE_linux.c
        )

target_link_libraries(test_CPABE ECC CPABE LinSupport)

target_compile_definitions(test_CPABE PUBLIC
        CPABE_SETUP CPABE_KEYGEN CPABE_ENCRYPTION CPABE_DECRYPTION
        CPABE_SMALL_SET
        TARGET_LINUX32
        CPABE192K2
        TPSSC
        )

setup_module(test_CPABE)

add_test(IntegrationCPABE test_CPABE)