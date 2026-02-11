file(GLOB_RECURSE SOURCES "*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})

include_directories("../../include")

add_executable(simple_scene ${SOURCES})

target_link_libraries(simple_scene scenes_loader_assimp graphics_glfw)