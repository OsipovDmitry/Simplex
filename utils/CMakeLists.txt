#project(utils)

file(GLOB_RECURSE HEADERS "../include/utils/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/utils PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/utils")

add_compile_definitions(UTILS_LIBRARY)

add_library(utils SHARED ${HEADERS} ${SOURCES})