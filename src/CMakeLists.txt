add_library(
  ${PROJECT}_lib STATIC
  asset_manager.hpp
  asset_manager.cpp
  component.hpp
  component.cpp
  context_aware_object.hpp
  color.hpp
  logging.hpp
  feature_flags.hpp
  file.hpp
  file.cpp
  font.hpp
  font.cpp
  framebuffer.hpp
  framebuffer.cpp
  game_clock.hpp
  game_clock.cpp
  gizmo_drawer.hpp
  gizmo_drawer.cpp
  gl_error_handler.hpp
  gl_error_handler.cpp
  graphics_buffer.hpp
  graphics_buffer.cpp
  image.hpp
  image.cpp
  input_system.hpp
  input_system.cpp
  light.hpp
  light.cpp
  material.hpp
  material.cpp
  mesh.hpp
  mesh.cpp
  shader.hpp
  shader.cpp
  vertex.hpp
  texture_viewer.hpp
  texture_viewer.cpp
  texture.hpp
  texture.cpp
  thread.hpp
  thread.cpp
  transform.hpp
  transform.cpp
  uniform_info.hpp
  utils.hpp
  vaomap.hpp
  vaomap.cpp
  experimental/input_system.hpp
  experimental/input_system.cpp
  experimental/viewport.hpp
  experimental/viewport.cpp
  experimental/window_events.hpp
  experimental/window_events.cpp
  experimental/window.hpp
  experimental/window.cpp
  asset_loaders/asset_loader.hpp)

set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
link_libraries(spdlog::spdlog profiler::lib)

add_subdirectory(glad)
add_subdirectory(common)
add_subdirectory(graphics)
add_subdirectory(renderer)
add_subdirectory(project)

target_precompile_headers(${PROJECT}_lib REUSE_FROM ${PROJECT}::common)

target_link_libraries(
  ${PROJECT}_lib
  PUBLIC ${PROJECT}::common
         ${PROJECT}::includes
         ${PROJECT}::graphics
         ${PROJECT}::project
         freetype
         Winmm
         filewatch)

add_library(${PROJECT}_includes INTERFACE)
add_library(${PROJECT}::includes ALIAS ${PROJECT}_includes)
target_include_directories(${PROJECT}_includes
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} glad/include)

macro(enable_asset_type)
  if(NOT ${PROJECT}_asset_loaders)
    add_custom_target(${PROJECT}_asset_loaders)
  endif()

  foreach(asset_type ${ARGN})
    string(TOLOWER ${asset_type} asset_type_lower)
    string(TOUPPER ${asset_type} asset_type_upper)
    message(STATUS "Enabling ${asset_type_upper} support")
    set(${PROJECT}_SUPPORT_${asset_type_upper} ON)
    target_link_libraries(${PROJECT}_lib
                          PRIVATE ${PROJECT}::asset::${asset_type_lower})
    add_dependencies(${PROJECT}_asset_loaders
                     ${PROJECT}::asset::${asset_type_lower})
  endforeach()
endmacro()

enable_asset_type(fbx jpg png mat shader)

add_subdirectory(asset_loaders)

add_library(${PROJECT}::lib ALIAS ${PROJECT}_lib)

add_executable(${PROJECT} main.cpp)
target_link_libraries(${PROJECT} PUBLIC ${PROJECT}::lib)

add_subdirectory(samples)
