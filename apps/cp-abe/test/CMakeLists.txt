cmake_minimum_required(VERSION 3.12)

add_executable(CPABE-test
        runTest.c

        qsortTest.c
        cpabeTest.c
        )

target_include_directories (CPABE-test
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(CPABE-test
        PRIVATE unity fff ECC-fakes CPABE)

add_test(CPABETest CPABE-test)
