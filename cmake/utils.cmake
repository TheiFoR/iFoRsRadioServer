include(${CMAKE_CURRENT_LIST_DIR}/copy.cmake)

# Define a function to remove the .qt folder after build
function(remove_qt_folder)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${CMAKE_BINARY_DIR}/.qt"
        COMMENT "Removing .qt folder after build"
    )
endfunction()

# Define a function to run windeployqt after build
function(run_windeployqt6)
    message([WinDeployQt6] "Run windeployqt6")
    message([WinDeployQt6] "Qt bin dir: ${QT_BIN_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${QT_BIN_DIR}/windeployqt"  # Use the windeployqt tool to deploy the application
        --dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}  # Specify the target directory
        --no-compiler-runtime  # Avoid copying the compiler runtime
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe"  # Path to the executable to deploy
        COMMENT "Running windeployqt for deployment"
    )
endfunction()
