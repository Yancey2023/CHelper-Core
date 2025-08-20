cmake_minimum_required(VERSION 3.21)
project(CHelperTest LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Compile warning
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
elseif (MSVC)
    add_compile_options("/W4")
    add_compile_options("/wd4100")
endif ()

# Fix issues in MSVC
if (MSVC)
    add_compile_options("/utf-8")
endif ()

# ThirdParty: GTest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(include/googletest)
if (MSVC)
    target_compile_options(gtest PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
    target_compile_options(gtest_main PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()

if (NOT TARGET CHelperCore)
    message(FATAL_ERROR "CHelper-Core not found.")
endif ()

# Using GTest to test codes
file(GLOB_RECURSE TEST_FILE src/*.h src/*.cpp)
add_executable(CHelperTest ${TEST_FILE})
target_link_libraries(CHelperTest PRIVATE CHelper::Core GTest::gtest_main)
if (MSVC)
    target_compile_options(CHelperTest PRIVATE $<$<CONFIG:>:/MT> $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif ()
