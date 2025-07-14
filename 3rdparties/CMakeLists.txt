#project(3rdparties)

function(print_all_targets DIR FOLDER_NAME)
    get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(TGT IN LISTS TGTS)
        set_target_properties(${TGT} PROPERTIES FOLDER "${FOLDER_NAME}")
    endforeach()

    get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)
    foreach(SUBDIR IN LISTS SUBDIRS)
        print_all_targets("${SUBDIR}" "${FOLDER_NAME}")
    endforeach()
endfunction()


add_subdirectory("openal-soft")
print_all_targets(openal-soft "3rdparties/openal-soft")

add_subdirectory("glfw")
print_all_targets(glfw "3rdparties/glfw")

add_subdirectory("bullet3")
print_all_targets(bullet3 "3rdparties/bullet3")

