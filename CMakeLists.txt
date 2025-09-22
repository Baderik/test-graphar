cmake_minimum_required(VERSION 3.5)

# Set extension name here
set(TARGET_NAME guck)

# DuckDB's extension distribution supports vcpkg. As such, dependencies can be added in ./vcpkg.json and then
# used in cmake with find_package. Feel free to remove or replace with other dependencies.
# Note that it should also be removed from vcpkg.json to prevent needlessly installing it..
find_package(OpenSSL REQUIRED)

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

# GraphAr

#log_stage("Setting up GraphAR...")

ExternalProject_Add(
        graphar
        PREFIX ${GRAPHAR_PREFIX_DIR}
        GIT_REPOSITORY https://github.com/apache/incubator-graphar.git
        # GIT_TAG main
        GIT_TAG 1a44bc343e0e846df129c78152f6310a1d678429
        SOURCE_SUBDIR cpp
        CMAKE_ARGS
        -DGRAPHAR_BUILD_STATIC=ON
        -DBUILD_ARROW_FROM_SOURCE=ON
        -DCMAKE_INSTALL_PREFIX=${GRAPHAR_INSTALL_DIR}
        INSTALL_DIR ${GRAPHAR_INSTALL_DIR}
)

add_library(graphar::graphar_static STATIC IMPORTED GLOBAL)

set_target_properties(graphar::graphar_static PROPERTIES
        IMPORTED_LOCATION "${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_STATIC_LIBRARY_SUFFIX}"
        )

add_dependencies(graphar::graphar_static graphar)


set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})
include_directories(src/include)

set(EXTENSION_SOURCES src/guck_extension.cpp)

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Link OpenSSL in both the static library as the loadable extension
target_link_libraries(${EXTENSION_NAME} OpenSSL::SSL OpenSSL::Crypto graphar::graphar_static)
target_link_libraries(${LOADABLE_EXTENSION_NAME} OpenSSL::SSL OpenSSL::Crypto graphar::graphar_static)

message("!!! libs here ${INSTALL_LIB_DIR}")

install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
