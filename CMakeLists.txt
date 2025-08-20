cmake_minimum_required(VERSION 3.21)
project(CHelper)

# using c++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Compile warning
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
elseif (MSVC)
    add_compile_options("/W4")
    add_compile_options("/wd4100")
endif ()

# Fix issues in MSVC
if (MSVC)
    add_compile_options("/utf-8")
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/permissive-")
endif ()

# CHelper Core
add_subdirectory(CHelper-Core)

# Console Application
add_subdirectory(CHelper-Cmd)

# Web Application
add_subdirectory(CHelper-Web)

# Windows Application
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_subdirectory(CHelper-Windows)
endif ()

# Qt Application
find_package(Qt6 COMPONENTS Core Gui Widgets)
if (Qt6_FOUND)
    add_subdirectory(CHelper-Qt)
endif ()

# Android Shared Libary (not support MSVC)
if (NOT MSVC)
    add_subdirectory(CHelper-Android)
endif ()

# Using GTest to test codes
add_subdirectory(CHelper-Test)
