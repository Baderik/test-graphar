cmake_minimum_required(VERSION 3.10)  # Обновите минимальную версию

# Set extension name here
set(TARGET_NAME guck)


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

# Arrow
#log_stage("Configuring Apache Arrow...")
message("Configuring Apache Arrow...")

ExternalProject_Add(
    arrow
    PREFIX ${ARROW_PREFIX_DIR}
    GIT_REPOSITORY https://github.com/apache/arrow.git
    GIT_TAG apache-arrow-17.0.0
    SOURCE_SUBDIR cpp
    CMAKE_ARGS
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        -DARROW_BUILD_SHARED=ON
        -DARROW_BUILD_STATIC=OFF
        -DARROW_BUILD_TESTS=OFF
        -DARROW_BUILD_BENCHMARKS=OFF
        -DARROW_BUILD_EXAMPLES=OFF
        -DARROW_DEPENDENCY_SOURCE=BUNDLED
        -DARROW_OPTIONAL_INSTALL=ON
        -DARROW_COMPUTE=ON
        -DARROW_DATASET=ON
        -DARROW_FILESYSTEM=ON
        -DARROW_S3=ON
        -DARROW_PARQUET=ON
        -DARROW_ORC=ON
        -DARROW_JSON=ON
        -DARROW_WITH_ZLIB=ON
        -DARROW_WITH_ZSTD=ON
        -DARROW_WITH_BROTLI=OFF
        -DARROW_WITH_BZ2=OFF
        -DARROW_WITH_LZ4=OFF
        -DARROW_WITH_SNAPPY=ON
        -DARROW_CSV=ON
        -DARROW_GANDIVA=OFF
        -DARROW_TESTING=OFF
        -DCMAKE_INSTALL_PREFIX=${ARROW_INSTALL_DIR}
    BUILD_ALWAYS OFF
    INSTALL_DIR ${ARROW_INSTALL_DIR}
    BUILD_BYPRODUCTS ${ARROW_LIB_DIR}/libarrow${CMAKE_SHARED_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_acero${CMAKE_SHARED_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_bundled_dependencies${CMAKE_SHARED_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_compute${CMAKE_SHARED_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libarrow_dataset${CMAKE_SHARED_LIBRARY_SUFFIX} ${ARROW_LIB_DIR}/libparquet${CMAKE_SHARED_LIBRARY_SUFFIX}
)

    add_library(arrow::arrow_shared SHARED IMPORTED GLOBAL)

file(MAKE_DIRECTORY "${ARROW_INSTALL_DIR}/include")
set_target_properties(arrow::arrow_shared PROPERTIES
        IMPORTED_LOCATION "${ARROW_LIB_DIR}/libarrow${CMAKE_SHARED_LIBRARY_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES ${ARROW_INSTALL_DIR}/include
        )

add_dependencies(arrow::arrow_shared arrow)

# GraphAr
#log_stage("Setting up GraphAR...")
message("Setting up GraphAR...")
message("VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")

# Сначала объявим ExternalProject
ExternalProject_Add(
    graphar
    PREFIX ${GRAPHAR_PREFIX_DIR}
    GIT_REPOSITORY https://github.com/apache/incubator-graphar.git
    GIT_TAG 1a44bc343e0e846df129c78152f6310a1d678429
    SOURCE_SUBDIR cpp
    CMAKE_ARGS
#        -DGRAPHAR_BUILD_STATIC=ON
#        -DUSE_STATIC_ARROW=ON
        -DCMAKE_INSTALL_PREFIX=${GRAPHAR_INSTALL_DIR}
        -DCMAKE_PREFIX_PATH=${ARROW_INSTALL_DIR}
#        "-DCMAKE_PREFIX_PATH=${ARROW_INSTALL_DIR};${CMAKE_PREFIX_PATH};${VCPKG_INSTALLED_DIR}/x64-linux-release/share/curl"
        -DCMAKE_BUILD_TYPE=Release
        -DProtobuf_INCLUDE_DIR=${ARROW_PREFIX_DIR}/src/arrow-build/protobuf_ep-install/include
        -DProtobuf_LIBRARIES=${ARROW_PREFIX_DIR}/src/arrow-build/protobuf_ep-install/lib/libprotobuf${CMAKE_STATIC_LIBRARY_SUFFIX}
        -DProtobuf_VERSION=21.3.0
        -DCMAKE_CXX_FLAGS=-fPIC
        -DCMAKE_C_FLAGS=-fPIC
        -DARROW_ORC=ON
        -DCMAKE_REQUIRE_FIND_PACKAGE_CURL=ON
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}
        -DVCPKG_MANIFEST_DIR=${VCPKG_MANIFEST_DIR}
#        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    INSTALL_DIR ${GRAPHAR_INSTALL_DIR}
    BUILD_BYPRODUCTS ${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_SHARED_LIBRARY_SUFFIX}
    DEPENDS arrow
)

# Создаем импортированную цель
add_library(graphar::graphar_shared SHARED IMPORTED)
set_target_properties(graphar::graphar_shared PROPERTIES
        IMPORTED_LOCATION "${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_SHARED_LIBRARY_SUFFIX}"
        BUILD_RPATH "${ARROW_LIB_DIR}"
        INSTALL_RPATH "${ARROW_LIB_DIR}"
        )

#set_target_properties(graphar::graphar_shared PROPERTIES
#        IMPORTED_LOCATION ${GRAPHAR_LIB_DIR}/libgraphar${CMAKE_STATIC_LIBRARY_SUFFIX}
#        INTERFACE_INCLUDE_DIRECTORIES "${GRAPHAR_INSTALL_DIR}/include"
#        INTERFACE_LINK_LIBRARIES "arrow::arrow_static"
#        POSITION_INDEPENDENT_CODE ON
#        )

# Добавляем зависимость, чтобы GraphAR собирался до основного проекта
add_dependencies(graphar::graphar_shared graphar arrow)


if (APPLE)
    list(APPEND EXT_LIBS "-Wl,--disable-new-dtags") # Use rpath for ubuntu build
endif()
set(CMAKE_BUILD_RPATH
        "@loader_path"
        "${GRAPHAR_LIB_DIR}"
        "${ARROW_LIB_DIR}"
        )

set(CMAKE_INSTALL_RPATH
        "@loader_path"
        "${GRAPHAR_LIB_DIR}"
        "${ARROW_LIB_DIR}"
        )

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# Включаем директории для заголовочных файлов
file(MAKE_DIRECTORY ${GRAPHAR_INSTALL_DIR}/include)
target_include_directories(graphar::graphar_shared INTERFACE ${GRAPHAR_INSTALL_DIR}/include)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})
#include_directories(src/include)
include_directories(src/include ${GRAPHAR_INSTALL_DIR}/include ${GRAPHAR_SOURCE_DIR}/cpp/thirdparty ${arrow_SOURCE_DIR}/cpp/src)

set(EXTENSION_SOURCES src/guck_extension.cpp
        src/functions/table/hop.cpp
        src/utils/func.cpp
        src/utils/global_log_manager.cpp
        src/utils/benchmark.cpp)

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

set(EXTENSION_INCLUDES
        $<BUILD_INTERFACE:${EXTENSION_ROOT_DIR}/include>
        $<BUILD_INTERFACE:${GRAPHAR_INSTALL_DIR}/include>
        $<BUILD_INTERFACE:${GRAPHAR_SOURCE_DIR}/cpp/thirdparty>
        $<BUILD_INTERFACE:${duckdb_SOURCE_DIR}/src/include>
        $<BUILD_INTERFACE:${arrow_SOURCE_DIR}/cpp/src>
        )

target_include_directories(${EXTENSION_NAME} PUBLIC ${EXTENSION_INCLUDES} $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(${LOADABLE_EXTENSION_NAME} PUBLIC ${EXTENSION_INCLUDES} $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

#add_dependencies(${EXTENSION_NAME} arrow::arrow_shared graphar::arrow_shared)
#add_dependencies(${LOADABLE_EXTENSION_NAME} arrow::arrow_shared graphar::arrow_shared)

set(EXT_LIBS
        arrow::arrow_shared
        graphar::graphar_shared
        )

if (NOT APPLE)
    list(APPEND EXT_LIBS "-Wl,--disable-new-dtags") # Use rpath for ubuntu build
endif()

target_link_libraries(${EXTENSION_NAME} duckdb_static ${EXT_LIBS})
target_link_libraries(${LOADABLE_EXTENSION_NAME} ${EXT_LIBS})


#target_link_libraries(${EXTENSION_NAME}
#        graphar::graphar_static
#        arrow::arrow_static
#        ${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_acero${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_bundled_dependencies${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_compute${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_dataset${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libparquet${CMAKE_STATIC_LIBRARY_SUFFIX}
#        )
#
#target_link_libraries(${LOADABLE_EXTENSION_NAME}
#        graphar::graphar_static
#        arrow::arrow_static
#        ${ARROW_LIB_DIR}/libarrow${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_acero${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_bundled_dependencies${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_compute${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libarrow_dataset${CMAKE_STATIC_LIBRARY_SUFFIX}
#        ${ARROW_LIB_DIR}/libparquet${CMAKE_STATIC_LIBRARY_SUFFIX}
#        )

# Добавляем include директории
target_include_directories(${EXTENSION_NAME} PRIVATE ${GRAPHAR_INSTALL_DIR}/include)
target_include_directories(${LOADABLE_EXTENSION_NAME} PRIVATE ${GRAPHAR_INSTALL_DIR}/include)

message("!!! libs here ${INSTALL_LIB_DIR}")

#list(APPEND INSTALL_LIB_DIR "${GRAPHAR_LIB_DIR}")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE PATH "Library installation directory")

install(
        TARGETS ${EXTENSION_NAME}
        EXPORT "${DUCKDB_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")