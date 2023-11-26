add_executable(
  gl
  logging.hpp
  file.hpp
  file.cpp
  # fontgen.hpp
  # fontgen.cpp
  main.cpp
  shader.hpp
  shader.cpp
  glad/src/gl.c)

target_link_libraries(gl PUBLIC glfw glm spdlog::spdlog freetype)
target_include_directories(gl PRIVATE glad/include)
