add_library(
  ${PROJECT}_components STATIC
  camera.hpp
  camera.cpp
  light.hpp
  light.cpp
  mesh_filter.hpp
  mesh_filter.cpp
  mesh_renderer.hpp
  mesh_renderer.cpp
  transform.hpp
  transform.cpp)
add_library(${PROJECT}::components ALIAS ${PROJECT}_components)

target_include_directories(
  ${PROJECT}_components
  PUBLIC ${PROJECT_SRC_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(
  ${PROJECT}_components
  PUBLIC ${PROJECT}::component_interface ${PROJECT}::common ${PROJECT}::core
         ${PROJECT}::assets ${PROJECT}::project ${PROJECT}::renderer)
