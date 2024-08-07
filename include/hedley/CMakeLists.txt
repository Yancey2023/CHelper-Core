cmake_minimum_required(VERSION 3.27.2)

# Attempt to be "fetch_content"-able so that it works well 
# with tools like CPM or other manual dependency management

# Set the project name and language
project(
  hedley
  VERSION 17.0.0
  DESCRIPTION "Header file designed to smooth over some platform-specific annoyances."
  HOMEPAGE_URL "https://github.com/cblck/hedley"
  LANGUAGES CXX C)

add_library(hedley INTERFACE include/hedley.h)

target_include_directories(hedley INTERFACE include)

# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# Adding the tests:
include(CTest)

if(HEDLEY_TESTING AND BUILD_TESTING)
  message(AUTHOR_WARNING "Building hedley tests.")
  add_subdirectory(test)
endif()
