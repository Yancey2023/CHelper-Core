cmake_minimum_required(VERSION 3.21)
project(CHelperAndroid
        VERSION 0.2.29
        DESCRIPTION "Command Helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release
set(CMAKE_BUILD_TYPE Release)

# CHelper-Core
if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Core ${CMAKE_BINARY_DIR}/include/CHelper-Core)
endif ()

# Param Deliver
set(CHelperDebug false)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Resource)
configure_file(${CHELPER_CORE_DIR}/include/param_deliver/CHelperCoreParamDeliver.h.in include/param_deliver/CHelperCoreParamDeliver.h @ONLY)

# Sources
file(GLOB_RECURSE CHELPER_CORE_SOURCE_FILE ${CHELPER_CORE_DIR}/src/chelper/*.h ${CHELPER_CORE_DIR}/src/chelper/*.cpp)

# Android Application
add_library(CHelperAndroid SHARED src/AndroidNative.cpp)
init_chelper_library(CHelperAndroid)
target_compile_definitions(CHelperAndroid PRIVATE CHelperAndroid=true)
if (PROJECT_IS_TOP_LEVEL)
    target_link_libraries(CHelper::AndroidCore PUBLIC android log)
else ()
    target_include_directories(CHelper::AndroidCore PUBLIC include/android)
endif ()
