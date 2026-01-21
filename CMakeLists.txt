project(simplex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

add_subdirectory("3rdparties")
add_subdirectory("utils")
add_subdirectory("core")
add_subdirectory("graphics_glfw")
add_subdirectory("loader_assimp")
#add_subdirectory("app")
#add_subdirectory("iblgenerator")
add_subdirectory("examples")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/resources/")