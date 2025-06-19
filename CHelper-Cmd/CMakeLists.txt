cmake_minimum_required(VERSION 3.21)
project(CHelperCmd LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Compile warning
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
elseif (MSVC)
    add_compile_options("/W4")
    add_compile_options("/wd4100")
endif()

# Fix issues in MSVC
if (MSVC)
    add_compile_options("/utf-8")
endif ()

# CHelper Core
if(NOT TARGET CHelperCore)
    message(FATAL_ERROR "CHelper-Core not found.")
endif()

# Console Application
add_executable(CHelperCmd src/cmd/CHelperCmd.h src/cmd/CHelperCmd.cpp)
target_link_libraries(CHelperCmd PRIVATE CHelper::Core)

if (MSVC)
    target_compile_options(CHelperCmd PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()
