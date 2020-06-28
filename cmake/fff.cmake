include(ExternalProject)

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

add_library(fff INTERFACE IMPORTED)
target_include_directories(fff INTERFACE ${PROJECT_BINARY_DIR}/fff/src/fff_ext)
