cmake_minimum_required(VERSION 3.21)
project(CHelperCmd
        VERSION 0.2.29
        DESCRIPTION "Command Helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
