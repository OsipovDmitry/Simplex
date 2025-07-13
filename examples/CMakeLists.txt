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


add_subdirectory("empty_window")
add_subdirectory("teapot")
add_subdirectory("two_windows")
add_subdirectory("shared_windows")
print_all_targets("." "examples")


