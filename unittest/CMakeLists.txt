include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    ${PROJECT}_ut
    sample.cpp
)
target_link_libraries(
    ${PROJECT}_ut
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT}_ut)
