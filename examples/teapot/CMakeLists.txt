file(GLOB_RECURSE SOURCES "*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})

include_directories("../../include")

add_executable(teapot ${SOURCES})

target_link_libraries(teapot graphics_glfw)