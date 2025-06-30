project(qt)

#find_package(Qt6 COMPONENTS Core REQUIRED)

file(GLOB_RECURSE HEADERS "../include/qt/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/qt PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/qt")

add_compile_definitions(QT_LIBRARY)

add_library(qt SHARED ${HEADERS} ${SOURCES})

target_link_libraries(qt utils core)