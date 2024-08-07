
# TODO add compiler warnings from tests

file(GLOB tests *.c)
foreach(test IN LISTS tests)
  get_filename_component(test_we "${test}" NAME_WE)
  add_executable(c_${test_we} ${test})
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${test_we}.cpp
    COMMAND ${CMAKE_COMMAND} -E copy ${test} ${CMAKE_CURRENT_BINARY_DIR}/${test_we}.cpp
    DEPENDS ${test}
    COMMENT "Copy .c file to .cpp to compile it with C++"
  )
  add_executable(cxx_${test_we} {CMAKE_CURRENT_BINARY_DIR}/${test_we}.cpp)
endforeach()

