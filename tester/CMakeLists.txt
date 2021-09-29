cmake_minimum_required(VERSION 3.16)

project(Contiki_CPABE
        VERSION ${PROJECT_VERSION}
        LANGUAGES C)

include(testCPABE_linux.cmake)
include(testTP_linux.cmake)