set(HEADERS memory_manager.hpp)
set(INTERFACE_HEADERS
    application_context.hpp
    game_object.hpp
    object.hpp
    scene.hpp
    serialization_utilities.hpp
    serializer.hpp
    serializer_json.hpp
    components/component_registry.hpp
    components/component.hpp
    components/transform.hpp)
set(SOURCES
    application_context.cpp
    game_object.cpp
    memory_manager.cpp
    object.cpp
    scene.cpp
    components/component.cpp
    components/transform.cpp)

foreach(interface_header ${INTERFACE_HEADERS})
  get_filename_component(dir ${interface_header} DIRECTORY)
  message(
    STATUS ${CMAKE_CURRENT_INSTALL_DIR}/include/project/${interface_header})
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_INSTALL_DIR}/include/project/${interface_header}
           PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_INSTALL_DIR}/include/project/${dir}
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_CURRENT_INSTALL_DIR}/${interface_header}
      ${CMAKE_CURRENT_INSTALL_DIR}/include/project/${interface_header})
endforeach()

add_library(${PROJECT}_project STATIC ${INTERFACE_HEADERS} ${HEADERS}
                                      ${SOURCES})
add_library(${PROJECT}::project ALIAS ${PROJECT}_project)
target_link_libraries(${PROJECT}_project PUBLIC EnTT::EnTT ${PROJECT}::common)
target_include_directories(
  ${PROJECT}_project
  # PUBLIC ${CMAKE_CURRENT_INSTALL_DIR}/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
