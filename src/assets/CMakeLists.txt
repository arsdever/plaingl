add_library(
  ${PROJECT}_assets STATIC
  asset.cpp
  asset.hpp
  asset_importer.cpp
  asset_importer.hpp
  type_importer.hpp
  asset_cache.cpp
  asset_cache.hpp
  asset_manager.cpp
  asset_manager.hpp)

add_library(${PROJECT}::assets ALIAS ${PROJECT}_assets)
target_precompile_headers(${PROJECT}_assets REUSE_FROM ${PROJECT}::common)
target_link_libraries(
  ${PROJECT}_assets
  PUBLIC ${PROJECT}::common ${PROJECT}::core
  PRIVATE ${PROJECT}::assets::importers)

add_subdirectory(importers)
