cmake_minimum_required(VERSION 3.21)
project(CHelperCore LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fix issues in MSVC
if (MSVC)
    add_compile_options("/utf-8")
endif ()

# ThirdParty: fmt
add_subdirectory(include/fmt)
if (MSVC)
    target_compile_options(fmt PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()

# ThirdParty: utf8cpp
add_subdirectory(include/utfcpp)

# ThirdParty: hedley
add_subdirectory(include/hedley)

# ThirdParty: serialization
add_subdirectory(include/serialization)

# ThirdParty: spdlog
set(SPDLOG_FMT_EXTERNAL ON)
add_subdirectory(include/spdlog)
if (MSVC)
    target_compile_options(spdlog PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()

# Param Deliver
if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(CHelperDebug true)
else ()
    set(CHelperDebug false)
endif ()
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Resource)
configure_file(include/param_deliver/ParamDeliver.h.in ${CMAKE_BINARY_DIR}/include/param_deliver/ParamDeliver.h @ONLY)

# Sources
file(GLOB_RECURSE CHELPER_CORE_SOURCE_H include/chelper/*.h)
file(GLOB_RECURSE CHELPER_CORE_SOURCE_FILE src/chelper/*.cpp)

# Setup
function(init_chelper_library target CHELPER_CORE_DIR)
    target_sources(${target} PRIVATE ${CHELPER_CORE_SOURCE_FILE})
    target_precompile_headers(${target} PRIVATE ${CHELPER_CORE_DIR}/include/pch/pch.h)
    target_include_directories(${target} PUBLIC
            ${CHELPER_CORE_DIR}/include/pch
            ${CHELPER_CORE_DIR}/include/chelper
            ${CMAKE_BINARY_DIR}/include/param_deliver
            ${CHELPER_CORE_DIR}/include/rapidjson/include
            ${CHELPER_CORE_DIR}/include/serialization
    )
    target_link_libraries(${target} PUBLIC fmt::fmt-header-only spdlog::spdlog_header_only utf8cpp hedley serialization::serialization)
endfunction()

# CHelper Core
add_library(CHelperCore STATIC)
init_chelper_library(CHelperCore ${CMAKE_CURRENT_SOURCE_DIR})
if (MSVC)
    target_compile_options(CHelperCore PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()
add_library(CHelper::Core ALIAS CHelperCore)

# CHelper Core (no filesystem)
add_library(CHelperNoFilesystemCore STATIC)
init_chelper_library(CHelperNoFilesystemCore ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(CHelperNoFilesystemCore PUBLIC CHELPER_NO_FILESYSTEM)
if (MSVC)
    target_compile_options(CHelperNoFilesystemCore PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()
add_library(CHelper::NoFilesystemCore ALIAS CHelperNoFilesystemCore)

# Install
install(TARGETS CHelperCore DESTINATION lib)
install(FILES CHELPER_CORE_SOURCE_H DESTINATION include)
