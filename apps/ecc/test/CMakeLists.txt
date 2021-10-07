cmake_minimum_required(VERSION 3.12)

add_executable(ECC-test
        runTest.c

        sha1Test.c
        )

target_include_directories (ECC-test
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(ECC-test
        PRIVATE unity fff ECC)

add_test(ECCTest ECC-test)


add_subdirectory(fakes)
