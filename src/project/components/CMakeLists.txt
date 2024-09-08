add_library(${PROJECT}_components STATIC component_registry.cpp
                                         component_registry.hpp)
add_library(${PROJECT}::components ALIAS ${PROJECT}_components)
target_include_directories(
  ${PROJECT}_components
  PUBLIC ${PROJECT_SRC_DIR}
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(
  ${PROJECT}_components PUBLIC ${PROJECT}::common ${PROJECT}::core
                               ${PROJECT}::renderer)

macro(define_component component_name)
  target_sources(${PROJECT}_components PUBLIC ${component_name}.hpp)
  set(header_only FALSE)

  set(args ${ARGN})
  list(LENGTH args length)
  if(${length} GREATER 0)
    list(GET args 0 header_only)
  endif()

  if(NOT ${header_only})
    target_sources(${PROJECT}_components PRIVATE ${component_name}.cpp)
  endif()

  # add component name into component_list file
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/component_list.hpp
       "DECLARE_COMPONENT(${component_name}),")
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/component_list_inc.hpp
       "#include \"${component_name}.hpp\"\n")
endmacro()

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/component_list_inc.hpp)
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/component_list.hpp)

define_component(camera)
define_component(light)
define_component(mesh_filter)
define_component(mesh_renderer)
define_component(transform)

target_link_libraries(${PROJECT}_components PRIVATE ${PROJECT}::assets)
