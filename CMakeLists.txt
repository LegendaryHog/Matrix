cmake_minimum_required(VERSION 3.18)

project(MX LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

include(FetchContent)
FetchContent_Declare(
    Vector
    GIT_REPOSITORY https://github.com/LegendaryHog/Vector.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(Vector)

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)
enable_testing()

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

include_directories(./include)

add_subdirectory(${vector_SOURCE_DIR})
add_subdirectory(unit_tests)
add_subdirectory(task)