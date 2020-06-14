cmake_minimum_required(VERSION 3.16)

add_library(LinSupport
        list.c
#        testCPABE.c
#        testTP.c
        watchdog.c
        )

target_include_directories (LinSupport
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#target_link_libraries (ECC LINK_PUBLIC ...)
