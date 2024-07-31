message(STATUS "- \"common\" unit tests")
enable_testing()

add_executable(${PROJECT}_common_ut file_ut.cpp)

target_link_libraries(${PROJECT}_common_ut GTest::gtest_main ${PROJECT}::common)

target_precompile_headers(${PROJECT}_common_ut REUSE_FROM ${PROJECT}::common)

include(GoogleTest)
gtest_discover_tests(${PROJECT}_common_ut DISCOVERY_MODE PRE_TEST
                     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
