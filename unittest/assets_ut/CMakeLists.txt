enable_testing()

# This variable is used in directory unittests
file(GLOB DIRECTORY_UNITTEST_FILES_IN_CURRENT_DIRECTORY
     "${CMAKE_CURRENT_LIST_DIR}/*")
list(LENGTH DIRECTORY_UNITTEST_FILES_IN_CURRENT_DIRECTORY
     DIRECTORY_UNITTEST_FILES_COUNT_IN_CURRENT_DIRECTORY)

add_compile_definitions(
  "DIRECTORY_UNITTEST_FILES_COUNT_IN_CURRENT_DIRECTORY=${DIRECTORY_UNITTEST_FILES_COUNT_IN_CURRENT_DIRECTORY}"
)
add_executable(${PROJECT}_assets_ut asset_manager_ut.cpp)

target_link_libraries(${PROJECT}_assets_ut GTest::gtest_main ${PROJECT}::assets)

target_precompile_headers(${PROJECT}_assets_ut REUSE_FROM ${PROJECT}::assets)

include(GoogleTest)
gtest_discover_tests(${PROJECT}_assets_ut DISCOVERY_MODE PRE_TEST
                     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
