add_executable(
  gl
  color.hpp
  logging.hpp
  file.hpp
  file.cpp
  font.hpp
  font.cpp
  gl_window.hpp
  gl_window.cpp
  main.cpp
  material_property.hpp
  material.hpp
  material.cpp
  shader.hpp
  shader.cpp
  text.hpp
  text.cpp
  thread.hpp
  thread.cpp
  glad/src/gl.c)

target_link_libraries(gl PUBLIC glfw glm spdlog::spdlog freetype)
target_include_directories(gl PRIVATE glad/include)
