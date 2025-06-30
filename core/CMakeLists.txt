project(core)

file(GLOB_RECURSE HEADERS "../include/core/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/core PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/core")

add_compile_definitions(CORE_LIBRARY)

add_library(core SHARED ${HEADERS} ${SOURCES})

target_link_libraries(core utils)