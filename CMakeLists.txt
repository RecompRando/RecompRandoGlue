cmake_minimum_required(VERSION 3.10)

project(APCpp-Glue)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")

add_library(APCpp-Glue SHARED 
    apcpp-yaml-config-exports.cpp
    apcpp-glue.cpp
    apcpp-glue.h
)

target_include_directories(${PROJECT_NAME} PRIVATE lib/APCpp)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
else()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
endif(WIN32)

add_subdirectory(lib/APCpp)
include_directories(lib/APCpp)

target_link_libraries(${PROJECT_NAME} APCpp-static)