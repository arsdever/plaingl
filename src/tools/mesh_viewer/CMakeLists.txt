add_library(${PROJECT}_tools_mesh_viewer STATIC mesh_viewer.hpp mesh_viewer.cpp)
add_library(${PROJECT}::tools::mesh_viewer ALIAS ${PROJECT}_tools_mesh_viewer)

target_link_libraries(
  ${PROJECT}_tools_mesh_viewer
  PUBLIC ${PROJECT}::lib
  PRIVATE ${PROJECT}::glad)
target_precompile_headers(${PROJECT}_tools_mesh_viewer REUSE_FROM
                          ${PROJECT}::common)
