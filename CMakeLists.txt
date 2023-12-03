cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
project(plaingl)

add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/glfw)
set(ASSIMP_WARNINGS_AS_ERRORS FALSE)
set(ASSIMP_BUILD_FBX_IMPORTER TRUE)
add_subdirectory(thirdparty/assimp)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/freetype)

add_subdirectory(src)
