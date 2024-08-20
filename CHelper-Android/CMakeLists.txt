cmake_minimum_required(VERSION 3.21)
project(CHelperAndroid
        VERSION 0.2.29
        DESCRIPTION "command helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CHelper-Core
set(CHELPER_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Core)

if (PROJECT_IS_TOP_LEVEL)
    # ThirdParty: hedley
    include_directories(${CHELPER_CORE_DIR}/include/hedley/include)
    #    add_subdirectory(${CHELPER_CORE_DIR}/include/hedley ${CMAKE_BINARY_DIR}/include/hedley)

    # ThirdParty: json
    add_subdirectory(${CHELPER_CORE_DIR}/include/json ${CMAKE_BINARY_DIR}/include/json)

    # ThirdParty: fmt
    add_subdirectory(${CHELPER_CORE_DIR}/include/fmt ${CMAKE_BINARY_DIR}/include/fmt)

    # ThirdParty: utf8cpp
    add_subdirectory(${CHELPER_CORE_DIR}/include/utfcpp ${CMAKE_BINARY_DIR}/include/utfcpp)
endif ()

# Param Deliver
set(CHelperDebug false)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Resource)
configure_file(${CHELPER_CORE_DIR}/include/param_deliver/CHelperCoreParamDeliver.h.in include/param_deliver/CHelperCoreParamDeliver.h @ONLY)

# Sources
file(GLOB_RECURSE CHELPER_CORE_SOURCE_FILE ${CHELPER_CORE_DIR}/src/chelper/*.h ${CHELPER_CORE_DIR}/src/chelper/*.cpp)

# Web Application
add_library(CHelperAndroid SHARED
        src/android/AndroidNative.cpp
        ${CHELPER_CORE_SOURCE_FILE}
)
if (PROJECT_IS_TOP_LEVEL)
    target_link_libraries(CHelperAndroid PRIVATE android log)
else ()
    target_include_directories(CHelperAndroid PRIVATE include/android)
endif ()
target_include_directories(CHelperAndroid PUBLIC ${CHELPER_CORE_DIR}/include/pch ${CHELPER_CORE_DIR}/include/chelper ${CMAKE_CURRENT_BINARY_DIR}/include)
target_precompile_headers(CHelperAndroid PRIVATE ${CHELPER_CORE_DIR}/include/pch/pch.h)
target_compile_definitions(CHelperAndroid PRIVATE CHelperAndroid=true)
target_link_libraries(CHelperAndroid PRIVATE fmt::fmt nlohmann_json::nlohmann_json utf8cpp hedley)
