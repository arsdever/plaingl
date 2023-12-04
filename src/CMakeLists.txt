add_executable(
  gl
  asset_manager.hpp
  asset_manager.cpp
  camera.hpp
  camera.cpp
  color.hpp
  component.hpp
  component.cpp
  logging.hpp
  event.hpp
  file.hpp
  file.cpp
  font.hpp
  font.cpp
  game_object.hpp
  game_object.cpp
  gl_window.hpp
  gl_window.cpp
  main.cpp
  material_property.hpp
  material.hpp
  material.cpp
  mesh_renderer.hpp
  mesh_renderer.cpp
  mesh.hpp
  mesh.cpp
  renderer.hpp
  scene.hpp
  scene.cpp
  shader.hpp
  shader.cpp
  text.hpp
  text.cpp
  texture.hpp
  texture.cpp
  thread.hpp
  thread.cpp
  transform.hpp
  transform.cpp
  glad/src/gl.c)

target_link_libraries(gl PUBLIC glfw glm assimp::assimp profiler::lib
                                spdlog::spdlog freetype)
target_include_directories(gl PRIVATE glad/include)
