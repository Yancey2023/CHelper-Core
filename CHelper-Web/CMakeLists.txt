cmake_minimum_required(VERSION 3.21)
project(CHelperWeb LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CHelper-Core
if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Core ${CMAKE_BINARY_DIR}/include/CHelper-Core)
endif ()

# Web Application
add_library(CHelperWeb src/CHelperWeb.cpp)
if (NOT PROJECT_IS_TOP_LEVEL)
    target_include_directories(CHelperWeb PRIVATE include/emscripten/system/include)
endif ()
target_link_libraries(CHelperWeb PRIVATE CHelper::NoFilesystemCore)
