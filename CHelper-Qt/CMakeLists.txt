cmake_minimum_required(VERSION 3.21)
project(CHelperQt
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
# because I want to use static link, so I add this
if (MSVC)
    target_compile_options(CHelperQt PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()
