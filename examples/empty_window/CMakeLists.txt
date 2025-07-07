file(GLOB_RECURSE SOURCES "*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})

include_directories("../../include")

add_executable(empty_window ${SOURCES})

target_link_libraries(empty_window graphics_glfw)