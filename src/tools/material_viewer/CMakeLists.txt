add_library(${PROJECT}_tools_material_viewer STATIC material_viewer.hpp material_viewer.cpp)
add_library(${PROJECT}::tools::material_viewer ALIAS ${PROJECT}_tools_material_viewer)

target_link_libraries(${PROJECT}_tools_material_viewer PUBLIC ${PROJECT}::lib
                                                          ${PROJECT}::graphics)
target_precompile_headers(${PROJECT}_tools_material_viewer REUSE_FROM
                          ${PROJECT}::common)
