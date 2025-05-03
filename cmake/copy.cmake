# Function to copy Qt libraries matching a pattern, only if they do not already exist
function(copy_qt_libraries lib_pattern target_dir)
    message(STATUS "Copying Qt libraries matching pattern: ${lib_pattern}")

    # Find all libraries that match the given pattern
    file(GLOB QT_LIBS "${QT_BIN_DIR}/qt6*${lib_pattern}*.dll")

    # Loop through the libraries and copy them to the target directory
    foreach(lib IN LISTS QT_LIBS)
        get_filename_component(lib_name ${lib} NAME)
        set(target_path "${target_dir}/${lib_name}")

        # Check if the file already exists before copying
        if(NOT EXISTS "${target_path}")
            message(STATUS "Copying library: ${lib}")
            file(COPY ${lib} DESTINATION ${target_dir})
        else()
            message(STATUS "Skipping existing file: ${lib_name}")
        endif()
    endforeach()
endfunction()

# Function to copy all Qt6 DLLs, but only if they are not already present
function(copy_all_qt6_dlls)
    file(GLOB QT_DLLS "${QT_BIN_DIR}/qt6*.dll")

    foreach(dll IN LISTS QT_DLLS)
        get_filename_component(dll_name ${dll} NAME)
        set(target_path "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dll_name}")

        if(NOT EXISTS "${target_path}")
            message(STATUS "Copying Qt DLL: ${dll}")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${dll}"
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
            )
        else()
            message(STATUS "Skipping existing file: ${dll_name}")
        endif()
    endforeach()
endfunction()

# Function to copy Qt DLLs based on a list of dependencies
function(copy_qt_dlls_from_deps dep_list)
    foreach(dep IN LISTS dep_list)
        file(GLOB QT_DLLS "${QT_BIN_DIR}/qt6*${dep}*.dll")

        foreach(dll IN LISTS QT_DLLS)
            get_filename_component(dll_name ${dll} NAME)
            set(target_path "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dll_name}")

            if(NOT EXISTS "${target_path}")
                message(STATUS "Copying Qt DLL: ${dll}")
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${dll}"
                    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                )
            else()
                message(STATUS "Skipping existing file: ${dll_name}")
            endif()
        endforeach()
    endforeach()
endfunction()

# Function to copy a directory, only if it does not already exist
function(copy_dir input_dir output_dir)
    if(NOT EXISTS "${output_dir}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${input_dir}"
            "${output_dir}"
        )
    else()
        message(STATUS "Skipping directory copy, already exists: ${output_dir}")
    endif()
endfunction()
