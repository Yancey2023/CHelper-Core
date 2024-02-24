cmake_minimum_required(VERSION 3.26)
project(CHelper)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_H src/*.h)
file(GLOB_RECURSE SOURCE_CPP src/*.cpp)
add_executable(CHelper ${SOURCE_H} ${SOURCE_CPP}
        src/android/AndroidNative.h)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC include)
target_precompile_headers(${CMAKE_PROJECT_NAME} PRIVATE include/pch.h)