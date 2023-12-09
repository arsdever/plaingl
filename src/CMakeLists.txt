add_executable(
  gl
  asset_manager.hpp
  asset_manager.cpp
  camera.hpp
  camera.cpp
  component.hpp
  component.cpp
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
  gl_error_handler.hpp
  gl_error_handler.cpp
  gl_window.hpp
  gl_window.cpp
  image.hpp
  image.cpp
  main.cpp
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
  glad/src/gl.c
  asset_loaders/asset_loader.hpp
  components/mesh_component.hpp
  components/mesh_component.cpp
  components/mesh_renderer_component.hpp
  components/mesh_renderer_component.cpp
  components/text_component.hpp
  components/text_component.cpp
  components/text_renderer_component.hpp
  components/text_renderer_component.cpp)

add_library(${PROJECT}_includes INTERFACE)
target_include_directories(${PROJECT}_includes
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} glad/include)

link_libraries(spdlog::spdlog)

target_link_libraries(gl PUBLIC ${PROJECT}_includes glfw glm profiler::lib
                                freetype)

macro(enable_asset_type)
  if(NOT ${PROJECT}_asset_loaders)
    add_custom_target(${PROJECT}_asset_loaders)
  endif()
  foreach(asset_type ${ARGN})
    string(TOLOWER ${asset_type} asset_type_lower)
    string(TOUPPER ${asset_type} asset_type_upper)
    message(STATUS "Enabling ${asset_type_upper} support")
    set(${PROJECT}_SUPPORT_${asset_type_upper} ON)
    target_link_libraries(gl PRIVATE ${PROJECT}::asset::${asset_type_lower})
    add_dependencies(${PROJECT}_asset_loaders
                     ${PROJECT}::asset::${asset_type_lower})
  endforeach()
endmacro()

enable_asset_type(fbx jpg png mat shader)

add_subdirectory(asset_loaders)
