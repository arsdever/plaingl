message(STATUS "- console tests")
enable_testing()

add_executable(${PROJECT}_console_ut console_ut.cpp)

target_link_libraries(${PROJECT}_console_ut GTest::gtest_main
                      ${PROJECT}::console ${PROJECT}::lib)

target_precompile_headers(${PROJECT}_console_ut REUSE_FROM ${PROJECT}::console)

include(GoogleTest)
gtest_discover_tests(${PROJECT}_console_ut)
