cmake_minimum_required(VERSION 3.21)
project(CHelperAndroid
        VERSION 0.2.29
        DESCRIPTION "Command Helper for Minecraft Bedrock Edition"
        LANGUAGES CXX)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Release
set(CMAKE_BUILD_TYPE Release)

# CHelper-Core
if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../CHelper-Core ${CMAKE_BINARY_DIR}/include/CHelper-Core)
endif ()

# Android Application
add_library(CHelperAndroid SHARED src/AndroidNative.cpp)
target_compile_definitions(CHelperAndroid PUBLIC CHelperAndroid)
if (PROJECT_IS_TOP_LEVEL)
    target_link_libraries(CHelperAndroid PUBLIC android log)
else ()
    target_include_directories(CHelperAndroid PUBLIC include/android)
endif ()
target_link_libraries(CHelperAndroid PRIVATE CHelper::Core)
