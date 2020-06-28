cmake_minimum_required(VERSION 3.16)

project(Contiki_CPABE
        VERSION 1.0.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

enable_testing()

# IDE support compiler database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
endif (CMAKE_COMPILER_IS_GNUCC)

# set a default build type if none was specified
set(default_build_type "RelWithDebInfo")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

set(build_type ${CMAKE_BUILD_TYPE})
string(TOLOWER ${build_type} build_type)

if (${build_type} STREQUAL debug)
    set(DEBUG_BUILD TRUE)
    message(STATUS "You are building in DEBUG mode")
else ()
    set(DEBUG_BUILD FALSE)
    message(STATUS "You are building in RELEASE mode")
endif ()

# add the cmake/ directory to the cmake module paths. Modules in there can be loaded with the include function.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(unity)
include(fff)
include(setup_module)

add_subdirectory(linux)
add_subdirectory(apps/ecc)
add_subdirectory(apps/cp-abe)
add_subdirectory(tester)


