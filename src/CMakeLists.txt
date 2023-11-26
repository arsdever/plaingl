add_executable(
  gl
  logging.hpp
  file.hpp
  file.cpp
  main.cpp
  shader.hpp
  shader.cpp
  glad/src/gl.c)

target_link_libraries(gl PUBLIC glfw spdlog::spdlog)
target_include_directories(gl PRIVATE glad/include)
