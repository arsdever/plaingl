link_libraries(profiler::lib)
set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  ${PROJECT}_lib STATIC
  component.hpp
  component.cpp
  color.hpp
  feature_flags.hpp
  game_clock.hpp
  game_clock.cpp
  gizmo_drawer.hpp
  gizmo_drawer.cpp
  gl_error_handler.hpp
  gl_error_handler.cpp
  texture_viewer.hpp
  texture_viewer.cpp
  thread.hpp
  thread.cpp
  transform.hpp
  transform.cpp)
add_library(${PROJECT}::lib ALIAS ${PROJECT}_lib)

add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(glad)
add_subdirectory(graphics)
add_subdirectory(renderer)
add_subdirectory(project)
add_subdirectory(tools)

target_precompile_headers(${PROJECT}_lib REUSE_FROM ${PROJECT}::common)

target_link_libraries(
  ${PROJECT}_lib
  PUBLIC ${PROJECT}::common
         ${PROJECT}::core
         ${PROJECT}::includes
         ${PROJECT}::graphics
         ${PROJECT}::project
         freetype
         Winmm)

add_library(${PROJECT}_includes INTERFACE)
add_library(${PROJECT}::includes ALIAS ${PROJECT}_includes)
target_include_directories(${PROJECT}_includes
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} glad/include)

add_executable(${PROJECT} main.cpp)
target_link_libraries(
  ${PROJECT}
  PUBLIC ${PROJECT}::lib
  PRIVATE ${PROJECT}::tools)

add_subdirectory(samples)
