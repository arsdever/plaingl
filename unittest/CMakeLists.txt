message(STATUS "Building tests")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)

set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

message(STATUS "Configuring unittests")

add_custom_target(${PROJECT}_unittests)

set(unit_test_index 0)
macro(add_test_subdirectory test_folder_name)
  message(STATUS "${unit_test_index}. ${test_folder_name}")
  add_subdirectory(${test_folder_name}_ut)
  math(EXPR unit_test_index "${unit_test_index}+1")
endmacro()

add_test_subdirectory(common)
add_test_subdirectory(core)
add_test_subdirectory(console)
add_test_subdirectory(project)
