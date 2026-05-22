file(GLOB_RECURSE SOURCES "*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})

include_directories("../../include")

add_executable(many_point_lights ${SOURCES})

target_link_libraries(many_point_lights scenes_loader_assimp graphics_glfw)