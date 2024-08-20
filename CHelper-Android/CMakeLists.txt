cmake_minimum_required(VERSION 3.27)
project(CHelperWeb
        VERSION 0.2.29
        DESCRIPTION "command helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ThirdParty: hedley
include_directories(../CHelper-Core/include/hedley)

# ThirdParty: json
include_directories(../CHelper-Core/include/json)

# ThirdParty: fmt
include_directories(../CHelper-Core/include/fmt)

# ThirdParty: utf8cpp
include_directories(../CHelper-Core/include/utf8cpp)

# Param Deliver
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CHelperDebug true)
else ()
    set(CHelperDebug false)
endif ()
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../resource)
configure_file(../CHelper-Core/include/param_deliver/CHelperCoreParamDeliver.h.in include/param_deliver/CHelperCoreParamDeliver.h @ONLY)

# Sources
file(GLOB_RECURSE CHELPER_CORE_SOURCE_FILE ../CHelper-Core/src/chelper/*.h ../CHelper-Core/src/chelper/*.cpp)

# Web Application
add_library(CHelperAndroid SHARED
        src/android/AndroidNative.cpp
        ${CHELPER_CORE_SOURCE_FILE}
)
if(NOT PROJECT_IS_TOP_LEVEL)
    target_include_directories(CHelperAndroid PRIVATE include/android)
endif()
target_include_directories(CHelperAndroid PUBLIC ../CHelper-Core/include/pch ../CHelper-Core/include/chelper ${CMAKE_CURRENT_BINARY_DIR}/include)
target_precompile_headers(CHelperAndroid PRIVATE ../CHelper-Core/include/pch/pch.h)
target_compile_definitions(CHelperAndroid PRIVATE CHelperAndroid=true)
target_link_libraries(CHelperAndroid PRIVATE fmt::fmt nlohmann_json::nlohmann_json utf8cpp hedley)
