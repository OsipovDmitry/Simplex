file(GLOB_RECURSE HEADERS "../include/loader_assimp/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/loader_assimp PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/loader_assimp")
include_directories("../3rdparties/assimp/include")

add_compile_definitions(LOADER_ASSIMP_LIBRARY)

add_library(loader_assimp SHARED ${HEADERS} ${SOURCES})

target_link_libraries(loader_assimp utils core assimp)
