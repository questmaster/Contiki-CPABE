cmake_minimum_required(VERSION 3.12)

add_library(ECC-fakes STATIC
        nnFake.c
        nn2Fake.c
        )

target_include_directories(ECC-fakes
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/../..
        )

target_compile_definitions(ECC-fakes PUBLIC ${ECC_COMPILE_DEFINITIONS})

target_link_libraries(ECC-fakes fff)

