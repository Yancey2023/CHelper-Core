cmake_minimum_required(VERSION 3.21)
project(CHelperTest
        VERSION 0.2.29
        DESCRIPTION "command helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix issues in MSVC
if (MSVC)
    add_compile_options("/utf-8")
endif ()

# ThirdParty: GTest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(include/googletest)

if(NOT TARGET CHelperCore)
    message(FATAL_ERROR "CHelper-Core not found.")
endif()

# Using GTest to test codes
file(GLOB_RECURSE TEST_FILE src/*.h src/*.cpp)
add_executable(CHelperTest ${TEST_FILE})
target_link_libraries(CHelperTest PRIVATE CHelper::Core GTest::gtest_main)
