cmake_minimum_required(VERSION 3.21)
project(CHelperCore
        VERSION 0.2.29
        DESCRIPTION "Command Helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix issues in MSVC
if (MSVC)
    add_compile_options("/utf-8")
endif ()

# ThirdParty: fmt
add_subdirectory(include/fmt)

# ThirdParty: json
add_subdirectory(include/json)

# ThirdParty: utf8cpp
add_subdirectory(include/utfcpp)

# ThirdParty: hedley
add_subdirectory(include/hedley)

# Param Deliver
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CHelperDebug true)
else ()
    set(CHelperDebug false)
endif ()
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Resource)
set(CPACK_VERSION_RELEASE "1.21.30.03")
set(CPACK_VERSION_BETA "1.21.40.22")
set(CPACK_VERSION_NETEASE "1.20.10.25")
configure_file(include/param_deliver/CHelperCoreParamDeliver.h.in include/param_deliver/CHelperCoreParamDeliver.h @ONLY)

# Sources
file(GLOB_RECURSE CHELPER_CORE_SOURCE_H include/chelper/*.h)
file(GLOB_RECURSE CHELPER_CORE_SOURCE_FILE src/chelper/*.cpp)

# CHelper Core
add_library(CHelperCore STATIC ${CHELPER_CORE_SOURCE_FILE})
target_precompile_headers(CHelperCore PRIVATE include/pch/pch.h)
target_include_directories(CHelperCore PUBLIC include/pch include/chelper ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(CHelperCore PUBLIC fmt::fmt nlohmann_json::nlohmann_json utf8cpp hedley)
add_library(CHelper::Core ALIAS CHelperCore)

# Install
install(TARGETS CHelperCore DESTINATION lib)
install(FILES CHELPER_CORE_SOURCE_H DESTINATION include)
