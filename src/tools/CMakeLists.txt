add_subdirectory(console)
add_subdirectory(mesh_viewer)
add_subdirectory(profiler)
add_subdirectory(texture_viewer)

add_library(${PROJECT}_tools INTERFACE)
add_library(${PROJECT}::tools ALIAS ${PROJECT}_tools)

target_link_libraries(
  ${PROJECT}_tools
  INTERFACE ${PROJECT}::tools::console ${PROJECT}::tools::profiler
            ${PROJECT}::tools::mesh_viewer ${PROJECT}::tools::texture_viewer)
