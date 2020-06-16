cmake_minimum_required(VERSION 3.12)

add_executable(CPABE-test
        qsortTest.c
        )

target_link_libraries(CPABE-test
        PRIVATE CPABE unity)

add_dependencies(CPABE-test unity_ext)

add_test(CPABETest CPABE-test)
