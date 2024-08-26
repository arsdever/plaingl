add_library(
  ${PROJECT}_core STATIC
  asset_manager.hpp
  asset_manager.cpp
  command_dispatcher.cpp
  command_dispatcher.hpp
  command.hpp
  input_system.hpp
  input_system.cpp
  settings.hpp
  settings.cpp
  viewport.hpp
  viewport.cpp
  window_events.hpp
  window_events.cpp
  window.hpp
  window.cpp
  asset_loaders/asset_loader.hpp)
add_library(${PROJECT}::core ALIAS ${PROJECT}_core)

target_link_libraries(${PROJECT}_core PUBLIC ${PROJECT}::common)

target_precompile_headers(${PROJECT}_core REUSE_FROM ${PROJECT}::common)

macro(enable_asset_type)
  if(NOT ${PROJECT}_asset_loaders)
    add_custom_target(${PROJECT}_asset_loaders)
  endif()

  foreach(asset_type ${ARGN})
    string(TOLOWER ${asset_type} asset_type_lower)
    string(TOUPPER ${asset_type} asset_type_upper)
    message(STATUS "Enabling ${asset_type_upper} support")
    set(${PROJECT}_SUPPORT_${asset_type_upper} ON)
    target_link_libraries(${PROJECT}_core
                          PRIVATE ${PROJECT}::asset::${asset_type_lower})
    add_dependencies(${PROJECT}_asset_loaders
                     ${PROJECT}::asset::${asset_type_lower})
  endforeach()
endmacro()

enable_asset_type(fbx jpg png mat shader)

add_subdirectory(asset_loaders)
