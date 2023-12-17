include(ExternalProject)

# Begin: External Third Party Library
ExternalProject_Add(
    unity_ext
    PREFIX ${PROJECT_BINARY_DIR}/unity
    DOWNLOAD_NAME unity_v2.5.1.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
    URL https://github.com/ThrowTheSwitch/Unity/archive/v2.5.1.tar.gz
 #   URL_HASH SHA256=a8c5e384f511a03c603bbecc9edc24d2cb4a916998d51a29cf2e3a2896920d03
    BUILD_IN_SOURCE TRUE
    CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX:STRING=${PROJECT_BINARY_DIR}/unity
    )


# The above ExternalProject_Add(...) construct wil take care of \
# 1. Downloading sources
# 2. Building Object files
# 3. Install under DCMAKE_INSTALL_PREFIX Directory

# Acquire Installation Directory of
ExternalProject_Get_Property (unity_ext install_dir)

# Begin: Importing Headers & Library of Third Party built using ExternalProject_Add(...)
# Include PATH that has headers required by Target Project
include_directories (${install_dir}/include/unity)

# Import librarues from External Project required by Target Project
add_library(unity STATIC IMPORTED )
set_target_properties(unity PROPERTIES IMPORTED_LOCATION ${install_dir}/lib/libunity.a)

add_dependencies(unity unity_ext)

# End: Importing Headers & Library of Third Party built using ExternalProject_Add(...)
# End: External Third Party Library
