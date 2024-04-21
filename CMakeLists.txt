cmake_minimum_required(VERSION 3.18)

project(Matrix)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)
enable_testing()

include(FetchContent)
FetchContent_Declare(
    Vector
    GIT_REPOSITORY https://github.com/LegendaryHog/Vector
    GIT_TAG        master
    OVERRIDE_FIND_PACKAGE
)
#FetchContent_Populate(Vector)

find_package(Vector REQUIRED)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory(${vector_SOURCE_DIR} ${vector_BINARY_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(unit_tests)
add_subdirectory(task)