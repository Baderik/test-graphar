cmake_minimum_required(VERSION 3.10)  # Обновите минимальную версию

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

# Arrow
#log_stage("Configuring Apache Arrow...")
message("Configuring Apache Arrow...")

ExternalProject_Add(
        arrow
        PREFIX ${ARROW_PREFIX_DIR}
        GIT_REPOSITORY https://github.com/apache/arrow.git
        GIT_TAG apache-arrow-21.0.0
        SOURCE_SUBDIR cpp
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${ARROW_INSTALL_DIR}
        -DCMAKE_INSTALL_LIBDIR=${ARROW_INSTALL_DIR}/lib
        -DCMAKE_INSTALL_INCLUDEDIR=${ARROW_INSTALL_DIR}/include
        -DCMAKE_INSTALL_BINDIR=${ARROW_INSTALL_DIR}/bin
        -DARROW_BUILD_SHARED=OFF
        -DARROW_BUILD_STATIC=ON
        -DARROW_BUILD_TESTS=OFF
        -DARROW_DEPENDENCY_SOURCE=BUNDLED
        -DARROW_COMPUTE=ON
        -DARROW_DATASET=ON
        -DARROW_FILESYSTEM=ON
        -DARROW_PARQUET=ON
        -DARROW_JSON=ON
        -DARROW_CSV=ON
#        -DARROW_WITH_ZLIB=ON
#        -DARROW_WITH_ZSTD=ON
#        -DARROW_WITH_SNAPPY=ON
        INSTALL_DIR ${ARROW_INSTALL_DIR}
        BUILD_BYPRODUCTS ${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX}
)

add_library(arrow::arrow_static STATIC IMPORTED GLOBAL)

file(MAKE_DIRECTORY "${ARROW_INSTALL_DIR}/include")
set_target_properties(arrow::arrow_static PROPERTIES
        IMPORTED_LOCATION "${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES ${ARROW_INSTALL_DIR}/include
        )

add_dependencies(arrow::arrow_static arrow)

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
        -DCMAKE_PREFIX_PATH=${ARROW_INSTALL_DIR}
        -DCMAKE_INSTALL_PREFIX=${GRAPHAR_INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
        INSTALL_DIR ${GRAPHAR_INSTALL_DIR}
        BUILD_BYPRODUCTS ${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_STATIC_LIBRARY_SUFFIX}
        DEPENDS arrow
)

# Создаем импортированную цель
add_library(graphar::graphar_static STATIC IMPORTED)
set_target_properties(graphar::graphar_static PROPERTIES
        IMPORTED_LOCATION ${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_STATIC_LIBRARY_SUFFIX}
        INTERFACE_INCLUDE_DIRECTORIES "${GRAPHAR_INSTALL_DIR}/include"
        INTERFACE_LINK_LIBRARIES "arrow::arrow_static"
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

set(EXTENSION_SOURCES src/guck_extension.cpp)

# Убедитесь, что эти функции определены (они должны быть предоставлены DuckDB)
if(NOT COMMAND build_static_extension)
    message(FATAL_ERROR "build_static_extension function not found")
endif()

if(NOT COMMAND build_loadable_extension)
    message(FATAL_ERROR "build_loadable_extension function not found")
endif()

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Link OpenSSL in both the static library as the loadable extension
# Добавляем зависимости, чтобы GraphAR собирался первым
add_dependencies(${EXTENSION_NAME} graphar::graphar_static)
add_dependencies(${LOADABLE_EXTENSION_NAME} graphar::graphar_static)

target_link_libraries(${EXTENSION_NAME}
        OpenSSL::SSL
        OpenSSL::Crypto
        graphar::graphar_static
        )

target_link_libraries(${LOADABLE_EXTENSION_NAME}
        OpenSSL::SSL
        OpenSSL::Crypto
        graphar::graphar_static
        )

# Добавляем include директории
target_include_directories(${EXTENSION_NAME} PRIVATE ${GRAPHAR_INSTALL_DIR}/include)
target_include_directories(${LOADABLE_EXTENSION_NAME} PRIVATE ${GRAPHAR_INSTALL_DIR}/include)

message("!!! libs here ${INSTALL_LIB_DIR}")

list(APPEND INSTALL_LIB_DIR "${GRAPHAR_LIB_DIR}")

install(
        TARGETS ${EXTENSION_NAME}
        EXPORT "${DUCKDB_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")