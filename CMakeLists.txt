# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.19)
message(STATUS "CMake version: ${CMAKE_VERSION}")

set(CMAKE_TOOLCHAIN_FILE F:/Other/C/Qt/Api/grpc/New_folder/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

# Define the project name and version
set(PROJECT_NAME iFoRsRadioServer)
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

# Ensure that the C++ standard required is applied for this project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automoc (automatic generation of MOC files for Qt classes)
set(CMAKE_AUTOMOC ON)

# Enable automatic handling of resource files (RCC) and UI files (UIC) for Qt
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set the root directory of the CMakeLists.txt file
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# Set the directory for Qt's binary files (e.g., Qt DLLs)
set(QT_BIN_DIR ${Qt6_DIR}/../../../bin)

# Set the directory where Qt plugins are located
set(QT_PLUGINS_DIR ${Qt6_DIR}/../../../plugins)

# Set the directory for Qt QML files
# set(QT_QML_DIR ${Qt6_DIR}/../../../qml)

# Enable QML compilation for the project (Qt Quick Compiler)
# set(QT_QML_COMPILER ON CACHE BOOL "Enable QML compiler")



# List of Qt dependencies to link with (can be expanded later with more components)
set(QT_DEP_LIST
    Core
    Network
)
set(DEP_LIST
    protobuf::libprotobuf
    gRPC::grpc++
    absl::strings
    absl::log
    absl::base
)
set(ALL_DEP_LIST
    ${DEP_LIST}
)
foreach(dep IN LISTS QT_DEP_LIST)
    list(APPEND ALL_DEP_LIST Qt6::${dep})
endforeach()



include("${ROOT_DIR}/cmake/utils.cmake")
include("${ROOT_DIR}/cmake/options.cmake")



# Loop through each dependency in QT_DEP_LIST and find the corresponding Qt component
foreach(dep IN LISTS QT_DEP_LIST)
    find_package(Qt6 REQUIRED COMPONENTS ${dep})  # Find the necessary Qt6 components
endforeach()

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)



# Include the current source directory to find header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories("F:/Other/C/Qt/Api/grpc/New_folder/vcpkg/installed/x64-windows/include")



# Recursively collect all source files in the specified directories
file(GLOB_RECURSE SRC
    "src/*.cpp"   # Collect all .cpp files in the src directory
    "src/*.h"     # Collect all .h files in the src directory
    "api/*.cpp"   # Collect all .cpp files in the api directory
    "api/*.h"     # Collect all .h files in the api directory
    ${CMAKE_CURRENT_BINARY_DIR}/*.h
    ${CMAKE_CURRENT_BINARY_DIR}/*.cc
)

# Set up the project for Qt 6.5 or higher, ensuring proper integration
qt_standard_project_setup(REQUIRES 6.5)

# Add Qt resources (e.g., images, QML files) from the resource file (resources.qrc)
# qt_add_resources(RESOURCE_FILES resources.qrc)

# Define the executable target for the project
qt_add_executable(${PROJECT_NAME}
    src/main.cpp # Main entry point for the application
)

# Add the source and resource files to the executable target
target_sources(${PROJECT_NAME}
    PRIVATE
    ${SRC}         # Add the collected source files
    ${RESOURCE_FILES}  # Add the collected resource files
)

foreach(dep IN LISTS ALL_DEP_LIST)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE ${dep} # Link all the required libraries from ALL_DEP_LIST
    )
endforeach()

# Set the runtime output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
# Set the library output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
# Set the archive output directory (for static libraries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Set target properties to ensure the executable is placed in the correct directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Set the executable to be a Windows application (without a console window)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE FALSE
)

# Include the GNUInstallDirs module for installation paths (e.g., bin, lib)
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

copy_dir("${QT_PLUGINS_DIR}/platforms"    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/platforms")
copy_dir("${QT_PLUGINS_DIR}/imageformats" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/imageformats")
copy_dir("${QT_PLUGINS_DIR}/styles"       "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins/styles")
# copy_dir("${QT_QML_DIR}"                  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qml")

copy_qt_dlls_from_deps(${QT_DEP_LIST})
if(WIN32)
    run_windeployqt6()
endif()
remove_qt_folder()
