project(openal)

#find_package(OpenAL REQUIRED)

add_subdirectory(openal-soft-master)
add_custom_target("qwe")


file(GLOB_RECURSE HEADERS "../include/openal/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/openal PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/openal")
#include_directories(${OPENAL_INCLUDE_DIR})

add_compile_definitions(OPENAL_LIBRARY)

add_library(openal SHARED ${HEADERS} ${SOURCES})

target_link_libraries(openal utils core OpenAL::OpenAL)
