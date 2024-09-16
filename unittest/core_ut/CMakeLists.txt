enable_testing()

add_executable(${PROJECT}_core_ut timer_ut.cpp)

target_link_libraries(${PROJECT}_core_ut GTest::gtest_main ${PROJECT}::core)

target_precompile_headers(${PROJECT}_core_ut REUSE_FROM ${PROJECT}::core)

include(GoogleTest)
gtest_discover_tests(${PROJECT}_core_ut DISCOVERY_MODE PRE_TEST
                     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
