file(GLOB_RECURSE SOURCES "*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${SOURCES})

include_directories("../../include")

add_executable(shared_windows ${SOURCES})

target_link_libraries(shared_windows graphics_glfw)