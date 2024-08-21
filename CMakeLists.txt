cmake_minimum_required(VERSION 3.5)
project(boo)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

if(BOO_BUILD_SHARED)
    message(STATUS "Building BOO shared")
    add_library(boo SHARED
            include/BOO/BOO.h
            src/BOO.cpp)
else()
    message(STATUS "Building BOO static")
    add_library(boo STATIC
            include/BOO/BOO.h
            src/BOO.cpp)
endif()
target_include_directories(boo PRIVATE include)

if(PROJECT_IS_TOP_LEVEL)
    project(boo_test)

    add_executable(boo_test test/main.cpp)
    target_link_libraries(boo_test boo)
    target_include_directories(boo_test PRIVATE include)
endif()