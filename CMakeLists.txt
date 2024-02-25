cmake_minimum_required(VERSION 3.27)
project(CHelper)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_H src/chelper/*.h)
file(GLOB_RECURSE SOURCE_CPP src/chelper/*.cpp)

# 可执行文件
add_executable(CHelper src/main.h src/main.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelper PUBLIC include)
target_precompile_headers(CHelper PRIVATE include/pch.h)

# 静态链接库
#add_library(CHelperStatic STATIC ${SOURCE_H} ${SOURCE_CPP})
#target_include_directories(CHelperStatic PUBLIC include)
#target_precompile_headers(CHelperStatic PRIVATE include/pch.h)

# 动态链接库
#add_library(CHelperShared SHARED ${SOURCE_H} ${SOURCE_CPP})
#target_include_directories(CHelperShared PUBLIC include)
#target_precompile_headers(CHelperShared PRIVATE include/pch.h)