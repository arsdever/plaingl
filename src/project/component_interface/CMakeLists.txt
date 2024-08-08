add_library(
  ${PROJECT}_component_interface STATIC
  component_registry.hpp component_registry.cpp component.hpp component.cpp)
add_library(${PROJECT}::component_interface ALIAS
            ${PROJECT}_component_interface)

include(GenerateExportHeader)
generate_export_header(
  ${PROJECT}_component_interface EXPORT_MACRO_NAME COMPONENT_INTERFACE_EXPORT
  EXPORT_FILE_NAME component_interface_export.hpp)

target_include_directories(
  ${PROJECT}_component_interface
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE
    $<TARGET_PROPERTY:${PROJECT}::components,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(
  ${PROJECT}_component_interface
  PUBLIC ${PROJECT}::common
  PUBLIC EnTT::EnTT)

target_precompile_headers(${PROJECT}_component_interface REUSE_FROM
                          ${PROJECT}::common)
