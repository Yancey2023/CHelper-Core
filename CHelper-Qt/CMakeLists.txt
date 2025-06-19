cmake_minimum_required(VERSION 3.21)
project(CHelperQt LANGUAGES CXX)

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
    add_compile_options("/Zc:__cplusplus")
    add_compile_options("/permissive-")
endif ()

# ThirdParty: Qt6
find_package(Qt6 COMPONENTS Core Gui Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (NOT TARGET CHelperCore)
    message(FATAL_ERROR "CHelper-Core not found.")
endif ()

# Qt Application
add_executable(CHelperQt WIN32
        src/CHelperQt.h
        src/CHelperQt.cpp
        src/chelper.qrc
        src/chelper.ui
)
target_link_libraries(CHelperQt PRIVATE CHelper::Core Qt6::Core Qt6::Gui Qt6::Widgets)
if (MSVC)
    target_compile_options(CHelperQt PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()
