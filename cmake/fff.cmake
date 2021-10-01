include(ExternalProject)

# Begin: External Third Party Library
ExternalProject_Add(
    fff_ext
    PREFIX ${PROJECT_BINARY_DIR}/fff
    DOWNLOAD_NAME fff_v2.5.1.zip
    URL https://github.com/meekrosoft/fff/archive/master.zip
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
    )


# The above ExternalProject_Add(...) construct wil take care of \
# 1. Downloading sources
# 2. Building Object files
# 3. Install under DCMAKE_INSTALL_PREFIX Directory

# Acquire Installation Directory of
ExternalProject_Get_Property (fff_ext install_dir)

# Begin: Importing Headers & Library of Third Party built using ExternalProject_Add(...)
# Include PATH that has headers required by Target Project
include_directories (${install_dir}/src/fff_ext)

# Import librarues from External Project required by Target Project
add_library(fff INTERFACE IMPORTED )

add_dependencies(fff fff_ext)

# End: Importing Headers & Library of Third Party built using ExternalProject_Add(...)
# End: External Third Party Library
