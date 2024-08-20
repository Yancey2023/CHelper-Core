cmake_minimum_required(VERSION 3.27)
project(CHelperWindows
        VERSION 0.2.29
        DESCRIPTION "command helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix issues in MSVC
if (MSVC)
    add_compile_options("/utf-8")
endif ()

if(NOT TARGET CHelperCore)
    message(FATAL_ERROR "CHelper-Core not found.")
endif()

# Windows Application
add_executable(CHelperWindows
        src/windows/CHelperWindows.h
        src/windows/CHelperWindows.cpp
)
if (MSVC)
    set_target_properties(CHelperWindows PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()
target_link_libraries(CHelperWindows PRIVATE CHelper::Core)
