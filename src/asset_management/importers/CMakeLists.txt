add_library(
  ${PROJECT}_assets_importers STATIC
  material_importer.hpp
  material_importer.cpp
  model_importer.hpp
  model_importer.cpp
  shader_importer.hpp
  shader_importer.cpp
  texture_importer.hpp
  texture_importer.cpp)

add_library(${PROJECT}::assets::importers ALIAS ${PROJECT}_assets_importers)
target_precompile_headers(${PROJECT}_assets_importers REUSE_FROM
                          ${PROJECT}::common)
target_link_libraries(
  ${PROJECT}_assets_importers
  PUBLIC ${PROJECT}::common ${PROJECT}::core
  PRIVATE assimp::assimp ${PROJECT}::graphics nlohmann_json::nlohmann_json)
