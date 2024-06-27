add_library(
  ${PROJECT}_graphics STATIC
  # command_executors.cpp
  command_executors.hpp
  command.hpp
  context_aware_object.hpp
  gpu.cpp
  gpu.hpp
  graphics_buffer.cpp
  graphics_buffer.hpp
  graphics_channel.cpp
  graphics_channel.hpp
  image.cpp
  image.hpp
  mesh.cpp
  mesh.hpp
  material_property.hpp
  material.cpp
  material.hpp
  shader.cpp
  shader.hpp
  texture.cpp
  texture.hpp
  uniform_info.hpp
  utils.hpp
  vaomap.cpp
  vaomap.hpp
  vertex.hpp
  commands/command_type_traits.hpp
  commands/command_types.hpp
  commands/commands.hpp
  commands/draw_elements.hpp)
add_library(${PROJECT}::graphics ALIAS ${PROJECT}_graphics)

target_precompile_headers(${PROJECT}_graphics REUSE_FROM ${PROJECT}::common)
target_include_directories(
  ${PROJECT}_graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_SRC_DIR})
target_link_libraries(${PROJECT}_graphics PRIVATE ${PROJECT}::common
                                                  ${PROJECT}::glad)
