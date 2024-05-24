cmake_minimum_required(VERSION 3.27)
project(CHelper)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)

# Encoding: utf-8
if (MSVC)
    add_compile_options("/utf-8")
endif ()

# ThirdParty: GTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Sources
file(GLOB_RECURSE SOURCE_H src/chelper/*.h)
file(GLOB_RECURSE SOURCE_CPP src/chelper/*.cpp)

# Console Application
add_executable(CHelperCmd src/CHelperCmd.h src/CHelperCmd.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperCmd PUBLIC include)
target_precompile_headers(CHelperCmd PRIVATE include/pch.h)

# Web Application
add_library(CHelperWeb SHARED src/CHelperWeb.h src/CHelperWeb.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperWeb PUBLIC include)
target_precompile_headers(CHelperWeb PRIVATE include/pch.h)

# Windows Application
add_executable(CHelperWindows src/CHelperWindows.h src/CHelperWindows.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperWindows PUBLIC include)
target_precompile_headers(CHelperWindows PRIVATE include/pch.h)
if (MSVC)
    set_target_properties(CHelperWindows PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

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
include(GoogleTest)
enable_testing()
add_executable(CHelperTest ${TEST_H} ${TEST_CPP} ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperTest PUBLIC include)
target_precompile_headers(CHelperTest PRIVATE include/pch.h)
target_link_libraries(CHelperTest PUBLIC GTest::gtest_main)
gtest_discover_tests(CHelperTest)
