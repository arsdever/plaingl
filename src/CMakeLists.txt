add_executable(
  gl
  camera.hpp
  camera.cpp
  color.hpp
  logging.hpp
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
  mesh.hpp
  mesh.cpp
  scene.hpp
  scene.cpp
  shader.hpp
  shader.cpp
  text.hpp
  text.cpp
  thread.hpp
  thread.cpp
  glad/src/gl.c)

target_link_libraries(gl PUBLIC glfw glm spdlog::spdlog freetype)
target_include_directories(gl PRIVATE glad/include)
