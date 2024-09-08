add_library(
  ${PROJECT}_graphics STATIC
  context_aware_object.hpp
  framebuffer.hpp
  framebuffer.cpp
  font.cpp
  font.hpp
  # TODO: Fix gizmo drawer
  # gizmo_drawer.hpp
  # gizmo_drawer.cpp
  gl_error_handler.cpp
  gl_error_handler.hpp
  gpu.cpp
  gpu.hpp
  graphics_buffer.cpp
  graphics_buffer.hpp
  graphics.cpp
  graphics.hpp
  image.cpp
  image.hpp
  mesh.cpp
  mesh.hpp
  material.cpp
  material.hpp
  shader_property.hpp
  shader_script.cpp
  shader_script.hpp
  shader.cpp
  shader.hpp
  texture.cpp
  texture.hpp
  utils.hpp
  vaomap.cpp
  vaomap.hpp
  vertex.hpp
  formats/jpg.hpp
  formats/png.hpp)
add_library(${PROJECT}::graphics ALIAS ${PROJECT}_graphics)

add_subdirectory(glad)
add_subdirectory(renderer)

target_precompile_headers(${PROJECT}_graphics REUSE_FROM ${PROJECT}::common)
target_include_directories(
  ${PROJECT}_graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SRC_DIR})
target_link_libraries(
  ${PROJECT}_graphics
  PUBLIC ${PROJECT}::common ${PROJECT}::renderer
  PRIVATE ${PROJECT}::glad PNG::PNG JPEG::JPEG)
