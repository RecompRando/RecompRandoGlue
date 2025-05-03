cmake_minimum_required(VERSION 3.10)

project(APCpp-Glue)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")

# Include the script to download and set up the Python binary
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64-bit Windows
        include(${CMAKE_SOURCE_DIR}/PythonStandalone_Win_x64.cmake)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        # 32-bit Windows
        message(FATAL_ERROR "Building for 32-bit Windows not supported.")
    else()
        message(FATAL_ERROR "Unknown pointer size.")
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64-bit Linux
        include(${CMAKE_SOURCE_DIR}/PythonStandalone_Linux_x64.cmake)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        # 32-bit Linux
        message(FATAL_ERROR "Building for 32-bit Linux not supported.")
    else()
        message(FATAL_ERROR "Unknown pointer size.")
    endif()
endif()

add_library(APCpp-Glue SHARED 
    apcpp-yaml-config-exports.cpp
    apcpp-glue.cpp
    apcpp-solo-gen.cpp
    apcpp-glue.h
    apcpp-solo-gen.h
)

target_include_directories(APCpp-Glue PRIVATE lib/APCpp)

if (WIN32)
    target_link_libraries(APCpp-Glue PRIVATE ws2_32)
else()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
endif(WIN32)

add_subdirectory(lib/APCpp)
include_directories(lib/APCpp lib/APCpp/jsoncpp/include)

# Define paths to the outputs
set(MINIPELAGO_ZIP_C "${CMAKE_BINARY_DIR}/minipelago.zip.c")

# Ensure the Python script always runs and generates the C files
add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/dummy" "${MINIPELAGO_ZIP_C}"
    COMMAND "${PYTHON_EXE}" "${CMAKE_SOURCE_DIR}/minipelago/package.py" "${CMAKE_BINARY_DIR}"
    DEPENDS "${CMAKE_SOURCE_DIR}/minipelago/package.py" # Ensure the Python script is up-to-date
    COMMENT "Packaging zips and generating C arrays"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    VERBATIM
)

# Custom target to ensure the custom command runs every time
add_custom_target(generate_c_arrays ALL
    DEPENDS "${CMAKE_BINARY_DIR}/dummy"
)

add_dependencies(APCpp-Glue generate_c_arrays)
target_sources(APCpp-Glue PRIVATE "${MINIPELAGO_ZIP_C}")
target_link_libraries(APCpp-Glue PRIVATE APCpp-static python_standalone)
link_python_standalone(APCpp-Glue)
