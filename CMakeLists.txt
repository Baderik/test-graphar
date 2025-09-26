cmake_minimum_required(VERSION 3.10)  # Обновите минимальную версию

# Set extension name here
set(TARGET_NAME guck)

# DuckDB's extension distribution supports vcpkg. As such, dependencies can be added in ./vcpkg.json and then
# used in cmake with find_package. Feel free to remove or replace with other dependencies.
# Note that it should also be removed from vcpkg.json to prevent needlessly installing it..
#find_package(OpenSSL REQUIRED)
#find_package(CURL REQUIRED)
#find_package(CURL REQUIRED GLOBAL)
#message("(:-) Found CURL: ${CURL_VERSION}")
#message("(:-) Found CURL: ${DCURL_INCLUDE_DIR}")
#message("(:-) Found CURL: ${CURL_INCLUDE_DIR}")
#find_package(ZLIB REQUIRED)
#find_package(ZSTD REQUIRED)
#find_package(SNAPPY REQUIRED)
#find_package(LZ4 REQUIRED)
#find_package(Zstd REQUIRED)
#find_package(Snappy REQUIRED)
#find_package(ZLIB REQUIRED)
#find_package(mimalloc REQUIRED)
#find_library(CURL_LIB curl)


# =============================================
# Core Configuration
# =============================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# =============================================
# Dependency Management
# =============================================
set(DEPS_INSTALL_DIR ${CMAKE_BINARY_DIR}/_deps)
set(ARROW_INSTALL_DIR ${DEPS_INSTALL_DIR}/arrow-install)
set(GRAPHAR_INSTALL_DIR ${DEPS_INSTALL_DIR}/graphar-install)

set(ARROW_PREFIX_DIR ${DEPS_INSTALL_DIR}/arrow-prefix)
set(GRAPHAR_PREFIX_DIR ${DEPS_INSTALL_DIR}/graphar-prefix)

set(ARROW_LIB_DIR ${ARROW_INSTALL_DIR}/lib)
set(GRAPHAR_LIB_DIR ${GRAPHAR_INSTALL_DIR}/lib)

set(GRAPHAR_SOURCE_DIR ${DEPS_INSTALL_DIR}/graphar-src)

include(FetchContent)
include(GNUInstallDirs)
include(ExternalProject)

# =============================================
# Setup global CURL target
# =============================================
#set(CURL_ROOT /opt/homebrew/opt/curl)
#set(CURL_INCLUDE_DIR ${CURL_ROOT}/include)
#set(CURL_LIBRARY ${CURL_ROOT}/lib/libcurl.dylib)
#
#add_library(CURL::libcurl UNKNOWN IMPORTED GLOBAL)
#set_target_properties(CURL::libcurl PROPERTIES
#        IMPORTED_LOCATION ${CURL_LIBRARY}
#        INTERFACE_INCLUDE_DIRECTORIES ${CURL_INCLUDE_DIR}
#        )

# Arrow
#log_stage("Configuring Apache Arrow...")
message("Configuring Apache Arrow...")

#ExternalProject_Add(
#    arrow
#    PREFIX ${ARROW_PREFIX_DIR}
#    GIT_REPOSITORY https://github.com/apache/arrow.git
#    GIT_TAG apache-arrow-21.0.0
#    SOURCE_SUBDIR cpp
#    CMAKE_ARGS
#        -DCMAKE_INSTALL_PREFIX=${ARROW_INSTALL_DIR}
#        -DCMAKE_INSTALL_LIBDIR=${ARROW_INSTALL_DIR}/lib
#        -DCMAKE_INSTALL_INCLUDEDIR=${ARROW_INSTALL_DIR}/include
#        -DCMAKE_INSTALL_BINDIR=${ARROW_INSTALL_DIR}/bin
#        -DARROW_BUILD_SHARED=OFF
#        -DARROW_BUILD_STATIC=ON
#        -DARROW_BUILD_TESTS=OFF
#        -DARROW_DEPENDENCY_SOURCE=BUNDLED
#        -DARROW_COMPUTE=ON
#        -DARROW_DATASET=ON
#        -DARROW_FILESYSTEM=ON
#        -DARROW_S3=OFF
#        -DARROW_PARQUET=ON
#        -DARROW_ORC=ON
#        -DARROW_JSON=ON
#        -DARROW_CSV=ON
#        -DARROW_WITH_ZLIB=ON
#        -DARROW_WITH_ZSTD=ON
#        -DARROW_WITH_LZ4=ON
#        -DARROW_WITH_SNAPPY=ON
#        -DARROW_WITH_MIMALLOC=ON
#        -DARROW_DEPENDENCY_USE_SHARED=OFF
##        -DARROW_WITH_CURL=ON
##        -DCURL_INCLUDE_DIR=${CURL_INCLUDE_DIR}
##        -DCURL_LIBRARY=${CURL_LIBRARY}
#    INSTALL_DIR ${ARROW_INSTALL_DIR}
#    BUILD_BYPRODUCTS ${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_acero${CMAKE_STATIC_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_bundled_dependencies${CMAKE_STATIC_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_compute${CMAKE_STATIC_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_dataset${CMAKE_STATIC_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libparquet${CMAKE_STATIC_LIBRARY_SUFFIX}
#)

#add_library(arrow::arrow_static STATIC IMPORTED GLOBAL)

#file(MAKE_DIRECTORY "${ARROW_INSTALL_DIR}/include")
#set_target_properties(arrow::arrow_static PROPERTIES
#        IMPORTED_LOCATION "${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX}"
#        INTERFACE_INCLUDE_DIRECTORIES ${ARROW_INSTALL_DIR}/include
#        INTERFACE_LINK_LIBRARIES "curlpp"
#        )

#target_link_libraries(arrow::arrow_static INTERFACE curlpp)

#add_dependencies(arrow::arrow_static arrow)

# GraphAr
#log_stage("Setting up GraphAR...")
message("Setting up GraphAR...")

# Сначала объявим ExternalProject
ExternalProject_Add(
    graphar
    PREFIX ${GRAPHAR_PREFIX_DIR}
    GIT_REPOSITORY https://github.com/apache/incubator-graphar.git
    GIT_TAG 1a44bc343e0e846df129c78152f6310a1d678429
    SOURCE_SUBDIR cpp
    CMAKE_ARGS
        -DGRAPHAR_BUILD_STATIC=ON
        -DUSE_STATIC_ARROW=ON
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
#        -DCMAKE_PREFIX_PATH=${ARROW_INSTALL_DIR}
#        -DCURL_INCLUDE_DIR=/opt/homebrew/opt/curl/include
#        -DCURL_LIBRARY=/opt/homebrew/opt/curl/lib/libcurl.dylib
        -DCMAKE_INSTALL_PREFIX=${GRAPHAR_INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
#        -DProtobuf_INCLUDE_DIR=${ARROW_PREFIX_DIR}/src/arrow-build/protobuf_ep-install/include
#        -DProtobuf_LIBRARIES=${ARROW_PREFIX_DIR}/src/arrow-build/protobuf_ep-install/lib/libprotobuf${CMAKE_STATIC_LIBRARY_SUFFIX}
#        -DProtobuf_VERSION=21.3.0
        -DCMAKE_CXX_FLAGS=-fPIC
        -DCMAKE_C_FLAGS=-fPIC
#        -DARROW_ORC=ON
#        -DCURL_INCLUDE_DIR=${CURL_INCLUDE_DIR}
#        -DCURL_ROOT=${CURL_ROOT}
        -DBUILD_ARROW_FROM_SOURCE=ON
    INSTALL_DIR ${GRAPHAR_INSTALL_DIR}
    BUILD_BYPRODUCTS ${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_STATIC_LIBRARY_SUFFIX}
#    DEPENDS arrow
)

# Создаем импортированную цель
add_library(graphar::graphar_static STATIC IMPORTED)
set_target_properties(graphar::graphar_static PROPERTIES
        IMPORTED_LOCATION ${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_STATIC_LIBRARY_SUFFIX}
        INTERFACE_INCLUDE_DIRECTORIES "${GRAPHAR_INSTALL_DIR}/include"
#        INTERFACE_LINK_LIBRARIES "arrow::arrow_static"
        POSITION_INDEPENDENT_CODE ON
        )

# Добавляем зависимость, чтобы GraphAR собирался до основного проекта
add_dependencies(graphar::graphar_static graphar)

# Включаем директории для заголовочных файлов
file(MAKE_DIRECTORY ${GRAPHAR_INSTALL_DIR}/include)
target_include_directories(graphar::graphar_static INTERFACE ${GRAPHAR_INSTALL_DIR}/include)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})
include_directories(src/include)

set(EXTENSION_SOURCES src/guck_extension.cpp
        src/functions/scalar/simple.cpp)

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Добавляем зависимости, чтобы GraphAR собирался первым
add_dependencies(${EXTENSION_NAME}
#        arrow::arrow_static
        graphar::graphar_static)
add_dependencies(${LOADABLE_EXTENSION_NAME}
#        arrow::arrow_static
        graphar::graphar_static)

target_link_libraries(${EXTENSION_NAME}
        graphar::graphar_static
#        arrow::arrow_static
#        ${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_acero${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_bundled_dependencies${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_compute${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_dataset${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libparquet${CMAKE_STATIC_LIBRARY_SUFFIX}
        )

target_link_libraries(${LOADABLE_EXTENSION_NAME}
        graphar::graphar_static
#        arrow::arrow_static
#        ${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_acero${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_bundled_dependencies${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_compute${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_dataset${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libparquet${CMAKE_STATIC_LIBRARY_SUFFIX}
        )

# Добавляем include директории
target_include_directories(${EXTENSION_NAME} PRIVATE ${GRAPHAR_INSTALL_DIR}/include)
target_include_directories(${LOADABLE_EXTENSION_NAME} PRIVATE ${GRAPHAR_INSTALL_DIR}/include)

message("!!! libs here ${INSTALL_LIB_DIR}")

#list(APPEND INSTALL_LIB_DIR "${GRAPHAR_LIB_DIR}")

install(
        TARGETS ${EXTENSION_NAME}
        EXPORT "${DUCKDB_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")