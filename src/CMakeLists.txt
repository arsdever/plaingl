add_executable(gl main.cpp glad/src/gl.c)

target_link_libraries(gl PUBLIC glfw spdlog::spdlog)
target_include_directories(gl PRIVATE glad/include)
