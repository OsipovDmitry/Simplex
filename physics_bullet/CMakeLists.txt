#project(openal)

file(GLOB_RECURSE HEADERS "../include/physics_bullet/*")
file(GLOB_RECURSE SOURCES "src/*")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include/physics_bullet PREFIX "Headers" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Sources" FILES ${SOURCES})

include_directories("../include")
include_directories("../include/physics_bullet")
include_directories("../3rdparties/bullet3/src")

add_compile_definitions(PHYSICS_BULLET_LIBRARY)

add_library(physics_bullet SHARED ${HEADERS} ${SOURCES})

target_link_libraries(physics_bullet utils core BulletDynamics BulletCollision LinearMath)
