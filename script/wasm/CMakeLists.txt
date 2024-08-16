cmake_minimum_required(VERSION 3.27)
project(CHelper)

# using c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ThirdParty: hedley
include_directories(include/hedley/include)

# ThirdParty: json
include_directories(include/json/include)

# ThirdParty: fmt
add_subdirectory(include/fmt)
link_libraries(fmt::fmt)

# Sources
file(GLOB_RECURSE SOURCE_FILE src/chelper/*.h src/chelper/*.cpp)
include_directories(include/pch ${CMAKE_BINARY_DIR}/include)

# Web Application
add_library(CHelperWeb
        src/web/CHelperWeb.cpp
        ${SOURCE_FILE}
)
target_include_directories(CHelperWeb PUBLIC include/emscripten/system/include)
target_precompile_headers(CHelperWeb PRIVATE include/pch/pch.h)
target_compile_definitions(CHelperWeb PRIVATE CHelperWeb=true)
