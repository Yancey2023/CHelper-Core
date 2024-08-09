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
find_package(Qt6 COMPONENTS Core Gui Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ThirdParty: GTest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(include/googletest)

# ThirdParty: hedley
include_directories(include/hedley/include)

# ThirdParty: json
include_directories(include/json/include)

# ThirdParty: fmt
add_subdirectory(include/fmt)
link_libraries(fmt::fmt)

# Param Deliver
set(CPACK_VERSION_RELEASE "1.21.2.02")
set(CPACK_VERSION_BETA "1.21.30.21")
set(CPACK_VERSION_NETEASE "1.20.10.25")
configure_file(include/param_deliver.h.in include/param_deliver.h @ONLY)

# Sources
file(GLOB_RECURSE SOURCE_FILE src/chelper/*.h src/chelper/*.cpp)
include_directories(include/pch ${CMAKE_BINARY_DIR}/include)

# Console Application
add_executable(CHelperCmd
        src/cmd/CHelperCmd.h
        src/cmd/CHelperCmd.cpp
        ${SOURCE_FILE}
)
target_precompile_headers(CHelperCmd PRIVATE include/pch/pch.h)

# Web Application
add_library(CHelperWeb
        src/web/CHelperWeb.cpp
        ${SOURCE_FILE}
)
target_include_directories(CHelperWeb PUBLIC include/emscripten/system/include)
target_precompile_headers(CHelperWeb PRIVATE include/pch/pch.h)
target_compile_definitions(CHelperWeb PRIVATE CHelperWeb=true)

# Windows Application
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_executable(CHelperWindows
            src/windows/CHelperWindows.h
            src/windows/CHelperWindows.cpp
            ${SOURCE_FILE}
    )
    target_precompile_headers(CHelperWindows PRIVATE include/pch/pch.h)
    if (MSVC)
        set_target_properties(CHelperWindows PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    endif ()
endif ()

# Qt Application
add_executable(CHelperQt WIN32
        src/qt/CHelperQt.h
        src/qt/CHelperQt.cpp
        src/qt/chelper.qrc
        src/qt/chelper.ui
        ${SOURCE_FILE}
)
target_precompile_headers(CHelperQt PRIVATE include/pch/pch.h)
target_link_libraries(CHelperQt Qt6::Core Qt6::Gui Qt6::Widgets)

# Android Shared Libary (not support MSVC)
if (NOT MSVC)
    add_library(CHelperAndroid SHARED
            src/android/AndroidNative.cpp
            ${SOURCE_FILE}
    )
    target_include_directories(CHelperAndroid PUBLIC include/android)
    target_precompile_headers(CHelperAndroid PRIVATE include/pch/pch.h)
    target_compile_definitions(CHelperAndroid PRIVATE CHelperAndroid=true)
endif ()

# Using GTest to test codes
file(GLOB_RECURSE TEST_FILE src/test/*.h src/test/*.cpp)
add_executable(CHelperTest ${TEST_FILE} ${SOURCE_FILE})
target_precompile_headers(CHelperTest PRIVATE include/pch/pch.h)
target_link_libraries(CHelperTest PUBLIC GTest::gtest_main)
