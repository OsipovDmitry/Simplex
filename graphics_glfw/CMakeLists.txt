file(GLOB_RECURSE HEADERS "../include/graphics_glfw/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/graphics_glfw PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/graphics_glfw")
include_directories("../3rdparties/glad/include")
include_directories("../3rdparties/glfw/include")

add_compile_definitions(GRAPHICS_GLFW_LIBRARY)

add_library(graphics_glfw SHARED ${HEADERS} ${SOURCES})

target_link_libraries(graphics_glfw utils core glfw3)
