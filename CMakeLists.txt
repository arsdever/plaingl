cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
project(plaingl)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/spdlog)

add_subdirectory(src)
