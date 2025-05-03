# Define the output directories for the build files (binary, libraries, etc.)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}_Debug")  # Set the Debug output directory
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}")  # Set the Release output directory
else()
    set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}_Another")  # Default build directory
endif()
