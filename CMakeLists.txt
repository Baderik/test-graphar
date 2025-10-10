cmake_minimum_required(VERSION 3.10)

set(TARGET_NAME guck)

find_package(CURL REQUIRED)
find_package(Arrow REQUIRED)

find_package(ArrowDataset REQUIRED)
find_package(ArrowAcero REQUIRED)
find_package(Parquet REQUIRED)
find_package(graphar REQUIRED)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})
include_directories(src/include)

set(EXTENSION_SOURCES
        src/guck_extension.cpp
        src/functions/table/hop.cpp
        src/utils/func.cpp
        src/utils/global_log_manager.cpp
        src/utils/benchmark.cpp)

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

set(EXTENSION_LIBRARIES
        graphar
        Arrow::arrow_static)

target_link_libraries(${EXTENSION_NAME} ${EXTENSION_LIBRARIES})
target_link_libraries(${LOADABLE_EXTENSION_NAME} ${EXTENSION_LIBRARIES})

install(
        TARGETS ${EXTENSION_NAME}
        EXPORT "${DUCKDB_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
