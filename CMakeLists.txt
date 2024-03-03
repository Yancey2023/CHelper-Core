cmake_minimum_required(VERSION 3.27)
project(CHelper)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options("/utf-8")
endif ()

file(GLOB_RECURSE SOURCE_H src/chelper/*.h)
file(GLOB_RECURSE SOURCE_CPP src/chelper/*.cpp)

# 控制台应用，在终端运行
add_executable(CHelperCmd src/CHelperCmd.h src/CHelperCmd.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperCmd PUBLIC include)
target_precompile_headers(CHelperCmd PRIVATE include/pch.h)

# 带GUI界面
add_executable(CHelperWindows src/CHelperWindows.h src/CHelperWindows.cpp ${SOURCE_H} ${SOURCE_CPP})
target_include_directories(CHelperWindows PUBLIC include)
target_precompile_headers(CHelperWindows PRIVATE include/pch.h)
if (MSVC)
    set_target_properties(CHelperWindows PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif ()

# 这只是为了在写代码的时候获取代码提示，实际上并不是编译成静态链接库再放到安卓的工程中
add_library(CHelperAndroid STATIC
        src/android/AndroidNative.h
        ${SOURCE_H}
        ${SOURCE_CPP})
target_include_directories(CHelperAndroid PUBLIC include)
target_precompile_headers(CHelperAndroid PRIVATE include/pch.h)

# 静态链接库
#add_library(CHelperStatic STATIC ${SOURCE_H} ${SOURCE_CPP})
#target_include_directories(CHelperStatic PUBLIC include)
#target_precompile_headers(CHelperStatic PRIVATE include/pch.h)

# 动态链接库
#add_library(CHelperShared SHARED ${SOURCE_H} ${SOURCE_CPP})
#target_include_directories(CHelperShared PUBLIC include)
#target_precompile_headers(CHelperShared PRIVATE include/pch.h)