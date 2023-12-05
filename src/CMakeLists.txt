add_executable(
  gl
  asset_manager.hpp
  asset_manager.cpp
  camera.hpp
  camera.cpp
  color.hpp
  logging.hpp
  event.hpp
  file.hpp
  file.cpp
  font.hpp
  font.cpp
  game_object.hpp
  game_object.cpp
  gizmo_drawer.hpp
  gizmo_drawer.cpp
  gizmo_object.hpp
  gizmo_object.cpp
  gl_window.hpp
  gl_window.cpp
  main.cpp
  material_property.hpp
  material.hpp
  material.cpp
  mesh.hpp
  mesh.cpp
  mouse_events_refiner.hpp
  mouse_events_refiner.cpp
  scene.hpp
  scene.cpp
  shader.hpp
  shader.cpp
  vertex.hpp
  text.hpp
  text.cpp
  texture.hpp
  texture.cpp
  thread.hpp
  thread.cpp
  transform.hpp
  transform.cpp
  glad/src/gl.c
  asset_loaders/asset_loader.hpp
  asset_loaders/fbx.hpp
  asset_loaders/fbx.cpp)

target_include_directories(gl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(gl PUBLIC glfw glm assimp::assimp profiler::lib
                                spdlog::spdlog freetype)
target_include_directories(gl PRIVATE glad/include)
