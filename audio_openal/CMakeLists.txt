#project(openal)

file(GLOB_RECURSE HEADERS "../include/audio_openal/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/audio_openal PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/audio_openal")

add_compile_definitions(AUDIO_OPENAL_LIBRARY)

add_library(audio_openal SHARED ${HEADERS} ${SOURCES})

target_link_libraries(audio_openal utils core OpenAL)
