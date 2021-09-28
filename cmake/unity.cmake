include(ExternalProject)

ExternalProject_Add(
    unity_ext
    PREFIX ${PROJECT_BINARY_DIR}/unity
    DOWNLOAD_NAME unity_v2.5.1.tar.gz
    URL https://github.com/ThrowTheSwitch/Unity/archive/v2.5.1.tar.gz
 #   URL_HASH SHA256=a8c5e384f511a03c603bbecc9edc24d2cb4a916998d51a29cf2e3a2896920d03
    BUILD_IN_SOURCE TRUE
    CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX:STRING=${PROJECT_BINARY_DIR}/unity
    )

add_library(unity STATIC IMPORTED GLOBAL)
set_target_properties(unity PROPERTIES IMPORTED_LOCATION ${PROJECT_BINARY_DIR}/unity/lib/libunity.a)
target_include_directories(unity INTERFACE ${PROJECT_BINARY_DIR}/unity/include/unity)
