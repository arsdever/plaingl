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

add_definitions(-w)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/glfw)
set(ASSIMP_WARNINGS_AS_ERRORS
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER
    ON
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/assimp)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/freetype)

set(PROF_OPENGL TRUE)
add_subdirectory(thirdparty/prof)

add_library(filewatch INTERFACE IMPORTED)
target_include_directories(filewatch INTERFACE "thirdparty/filewatch")

remove_definitions(-w)

add_subdirectory(src)

target_link_libraries(profiler_lib PUBLIC ${PROJECT}::glad)

if(ENABLE_TESTING)
  set(${PROJECT}_TESTING_ENABLED ON)
endif()

if(${PROJECT}_TESTING_ENABLED)
  add_subdirectory(test)
endif()
