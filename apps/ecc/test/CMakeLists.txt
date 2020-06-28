cmake_minimum_required(VERSION 3.12)

#add_executable(ECC-test
#        )
#
#target_link_libraries(ECC-test
#        PRIVATE ECC unity)
#
#add_dependencies(ECC-test unity_ext)
#
#add_test(ECCTest ECC-test)

include(../ecc_config.cmake)

add_subdirectory(fakes)
