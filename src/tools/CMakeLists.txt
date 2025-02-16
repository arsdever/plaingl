add_subdirectory(console)
add_subdirectory(material_viewer)
add_subdirectory(mesh_viewer)
add_subdirectory(profiler)
add_subdirectory(texture_viewer)

add_library(${PROJECT}_tools INTERFACE)
add_library(${PROJECT}::tools ALIAS ${PROJECT}_tools)

target_link_libraries(
  ${PROJECT}_tools
  INTERFACE ${PROJECT}::tools::console ${PROJECT}::tools::profiler
  ${PROJECT}::tools::material_viewer ${PROJECT}::tools::mesh_viewer ${PROJECT}::tools::texture_viewer)
