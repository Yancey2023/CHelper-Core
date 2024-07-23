cmake_minimum_required(VERSION 3.27)
project(CHelper)

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
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ThirdParty: GTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
enable_testing()

# Param Deliver
configure_file(include/param_deliver.h.in include/param_deliver.h @ONLY)

# Sources
file(GLOB_RECURSE SOURCE_H src/chelper/*.h)
file(GLOB_RECURSE SOURCE_CPP src/chelper/*.cpp)

# Console Application
add_executable(CHelperCmd
        src/cmd/CHelperCmd.h
        src/cmd/CHelperCmd.cpp
        ${SOURCE_H}
        ${SOURCE_CPP}
)
target_include_directories(CHelperCmd PUBLIC include ${CMAKE_BINARY_DIR}/include)
target_precompile_headers(CHelperCmd PRIVATE include/pch.h)

# Web Application
add_library(CHelperWeb
        src/web/CHelperWeb.cpp
        ${SOURCE_H}
        ${SOURCE_CPP}
)
target_include_directories(CHelperWeb PUBLIC include)
target_precompile_headers(CHelperWeb PRIVATE include/pch.h)

# Windows Application
add_executable(CHelperWindows
        src/windows/CHelperWindows.h
        src/windows/CHelperWindows.cpp
        ${SOURCE_H}
        ${SOURCE_CPP}
)
target_include_directories(CHelperWindows PUBLIC include ${CMAKE_BINARY_DIR}/include)
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
target_link_libraries(CHelperQt Qt6::Core Qt6::Gui Qt6::Widgets)

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
target_include_directories(CHelperTest PUBLIC include ${CMAKE_BINARY_DIR}/include)
target_precompile_headers(CHelperTest PRIVATE include/pch.h)
target_link_libraries(CHelperTest PUBLIC GTest::gtest_main)
gtest_discover_tests(CHelperTest)
