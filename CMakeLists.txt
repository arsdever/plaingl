cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(PROJECT gamify)

project(${PROJECT} LANGUAGES C CXX)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(spdlog REQUIRED)
find_package(freetype REQUIRED)

add_subdirectory(thirdparty/prof)

add_library(filewatch INTERFACE IMPORTED)
target_include_directories(filewatch INTERFACE "thirdparty/filewatch")

add_subdirectory(src)

target_link_libraries(profiler_lib PUBLIC ${PROJECT}::glad)

if(ENABLE_TESTING)
  set(${PROJECT}_TESTING_ENABLED ON)
endif()

if(${PROJECT}_TESTING_ENABLED)
  add_subdirectory(test)
endif()
