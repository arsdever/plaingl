enable_testing()

add_executable(${PROJECT}_project_ut game_object.cpp component_management.cpp)

target_link_libraries(${PROJECT}_project_ut GTest::gtest_main GTest::gmock
    ${PROJECT}::project)

target_precompile_headers(${PROJECT}_project_ut REUSE_FROM ${PROJECT}::project)

include(GoogleTest)
gtest_discover_tests(${PROJECT}_project_ut DISCOVERY_MODE PRE_TEST)
