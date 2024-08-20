cmake_minimum_required(VERSION 3.21)
project(CHelperQt
        VERSION 0.2.29
        DESCRIPTION "command helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

if(NOT TARGET CHelperCore)
    message(FATAL_ERROR "CHelper-Core not found.")
endif()

# Qt Application
add_executable(CHelperQt WIN32
        src/qt/CHelperQt.h
        src/qt/CHelperQt.cpp
        src/qt/chelper.qrc
        src/qt/chelper.ui
)
target_link_libraries(CHelperQt PRIVATE CHelper::Core Qt6::Core Qt6::Gui Qt6::Widgets)
