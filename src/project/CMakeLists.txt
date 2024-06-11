set(HEADERS memory_manager.hpp)
set(INTERFACE_HEADERS
    application_context.hpp game_object.hpp object.hpp scene.hpp
    components/component.hpp components/transform.hpp)
set(SOURCES
    application_context.cpp
    game_object.cpp
    memory_manager.cpp
    object.cpp
    scene.cpp
    components/component.cpp
    components/transform.cpp)

add_custom_target(${PROJECT}_project_includes)
foreach(interface_header ${INTERFACE_HEADERS})
  get_filename_component(dir ${interface_header} DIRECTORY)
  add_custom_command(
    TARGET ${PROJECT}_project_includes
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_INSTALL_DIR}/include/project/${dir}
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_CURRENT_SOURCE_DIR}/${interface_header}
      ${CMAKE_CURRENT_INSTALL_DIR}/include/project/${interface_header})
endforeach()

add_library(${PROJECT}_project STATIC ${INTERFACE_HEADERS} ${HEADERS}
                                      ${SOURCES})
add_library(${PROJECT}::project ALIAS ${PROJECT}_project)
target_link_libraries(${PROJECT}_project PUBLIC EnTT::EnTT ${PROJECT}::common)
target_include_directories(${PROJECT}_project
                           PUBLIC ${CMAKE_CURRENT_INSTALL_DIR}/include)
add_dependencies(${PROJECT}_project ${PROJECT}_project_includes)
