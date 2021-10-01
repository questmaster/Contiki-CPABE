cmake_minimum_required(VERSION 3.12)

include(../../ecc_config.cmake)

add_library(ECC-fakes STATIC
        nn2Fake.c
        )

target_include_directories(ECC-fakes
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/../..
        )

target_link_libraries(ECC-fakes fff)
