cmake_minimum_required(VERSION 3.27)
project(CHelperWeb
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
    add_subdirectory(${CHELPER_CORE_DIR}/include/hedley ${CMAKE_BINARY_DIR}/include/hedley)

    # ThirdParty: json
    add_subdirectory(${CHELPER_CORE_DIR}/include/json ${CMAKE_BINARY_DIR}/include/json)

    # ThirdParty: fmt
    add_subdirectory(${CHELPER_CORE_DIR}/include/fmt ${CMAKE_BINARY_DIR}/include/fmt)

    # ThirdParty: utf8cpp
    add_subdirectory(${CHELPER_CORE_DIR}/include/utfcpp ${CMAKE_BINARY_DIR}/include/utfcpp)
endif ()

# Param Deliver
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CHelperDebug true)
else ()
    set(CHelperDebug false)
endif ()
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Resource)
configure_file(${CHELPER_CORE_DIR}/include/param_deliver/CHelperCoreParamDeliver.h.in include/param_deliver/CHelperCoreParamDeliver.h @ONLY)

# Sources
file(GLOB_RECURSE CHELPER_CORE_SOURCE_FILE ${CHELPER_CORE_DIR}/src/chelper/*.h ${CHELPER_CORE_DIR}/src/chelper/*.cpp)

# Web Application
add_library(CHelperWeb
        src/web/CHelperWeb.cpp
        ${CHELPER_CORE_SOURCE_FILE}
)
if (NOT PROJECT_IS_TOP_LEVEL)
    target_include_directories(CHelperWeb PRIVATE include/emscripten/system/include)
endif ()
target_include_directories(CHelperWeb PUBLIC ${CHELPER_CORE_DIR}/include/pch ${CHELPER_CORE_DIR}/include/chelper ${CMAKE_CURRENT_BINARY_DIR}/include)
target_precompile_headers(CHelperWeb PRIVATE ${CHELPER_CORE_DIR}/include/pch/pch.h)
target_compile_definitions(CHelperWeb PRIVATE CHelperWeb=true)
target_link_libraries(CHelperWeb PRIVATE fmt::fmt nlohmann_json::nlohmann_json utf8cpp hedley)
