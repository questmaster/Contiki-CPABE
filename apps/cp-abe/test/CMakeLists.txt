cmake_minimum_required(VERSION 3.12)

add_executable(CPABE-test EXCLUDE_FROM_ALL
        qsortTest.c
        )

target_link_libraries(CPABE-test
        PRIVATE CPABE ${Unity_LIB})

add_dependencies(CPABE-test unity_ext)

add_test(CPABETest CPABE-test)
