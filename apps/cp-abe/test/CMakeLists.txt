cmake_minimum_required(VERSION 3.12)

add_executable(CPABE-test
        qsortTest.c
        qsortTest.h
        runTest.c
        cpabeTest.c
        cpabeTest.h
        )

target_link_libraries(CPABE-test
        PRIVATE unity fff ECC-fakes CPABE)

add_test(CPABETest CPABE-test)
