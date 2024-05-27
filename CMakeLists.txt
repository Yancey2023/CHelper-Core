cmake_minimum_required(VERSION 3.27)
project(CHelper)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)

# Encoding: utf-8
if (MSVC)
    add_compile_options("/utf-8")
endif ()

# ThirdParty: Qt5
find_package(Qt5 COMPONENTS
        Core Gui Widgets
        REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ThirdParty: GTest
find_package(GTest COMPONENTS REQUIRED)

# Sources
file(GLOB_RECURSE SOURCE_H src/chelper/*.h)
file(GLOB_RECURSE SOURCE_CPP src/chelper/*.cpp)

# Console Application
add_executable(CHelperCmd src/cmd/CHelperCmd.h src/cmd/CHelperCmd.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperCmd PUBLIC include)
target_precompile_headers(CHelperCmd PRIVATE include/pch.h)

# Web Application
add_library(CHelperWeb SHARED src/web/CHelperWeb.h src/web/CHelperWeb.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperWeb PUBLIC include)
target_precompile_headers(CHelperWeb PRIVATE include/pch.h)

# Windows Application
add_executable(CHelperWindows src/windows/CHelperWindows.h src/windows/CHelperWindows.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperWindows PUBLIC include)
target_precompile_headers(CHelperWindows PRIVATE include/pch.h)
if (MSVC)
    set_target_properties(CHelperWindows PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

# Qt Application
add_executable(CHelperQt WIN32
        src/qt/CHelperQt.h
        src/qt/CHelperQt.cpp
        src/qt/chelper.qrc
        src/qt/chelper.ui
        ${SOURCE_H}
        ${SOURCE_CPP}
)
target_include_directories(CHelperQt PUBLIC include)
target_precompile_headers(CHelperQt PRIVATE include/pch.h)
target_link_libraries(CHelperQt
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

# Android Shared Libary
add_library(CHelperAndroid SHARED
        src/android/AndroidNative.h
        ${SOURCE_H}
        ${SOURCE_CPP})
target_include_directories(CHelperAndroid PUBLIC include)
target_precompile_headers(CHelperAndroid PRIVATE include/pch.h)

# Using GTest to test codes
file(GLOB_RECURSE TEST_H src/test/*.h)
file(GLOB_RECURSE TEST_CPP src/test/*.cpp)
add_executable(CHelperTest ${TEST_H} ${TEST_CPP} ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperTest PUBLIC include)
target_precompile_headers(CHelperTest PRIVATE include/pch.h)
target_link_libraries(CHelperTest PUBLIC GTest::gtest_main)
gtest_discover_tests(CHelperTest)
