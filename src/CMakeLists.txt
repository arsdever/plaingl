set(LIB_SOURCES
    asset_manager.cpp
    camera.cpp
    file.cpp
    font.cpp
    game_object.cpp
    gizmo_drawer.cpp
    gizmo_object.cpp
    gl_window.cpp
    image.cpp
    material.cpp
    mesh.cpp
    mouse_events_refiner.cpp
    scene.cpp
    shader.cpp
    text.cpp
    texture_viewer.cpp
    texture.cpp
    thread.cpp
    transform.cpp
    glad/src/gl.c
    asset_loaders/fbx.cpp
    asset_loaders/png.cpp
    asset_loaders/mat.cpp
    asset_loaders/shader.cpp)

set(LIB_HEADERS
    asset_manager.hpp
    camera.hpp
    color.hpp
    logging.hpp
    event.hpp
    file.hpp
    font.hpp
    game_object.hpp
    gizmo_drawer.hpp
    gizmo_object.hpp
    gl_window.hpp
    image.hpp
    material.hpp
    mesh.hpp
    mouse_events_refiner.hpp
    scene.hpp
    shader.hpp
    vertex.hpp
    text.hpp
    texture_viewer.hpp
    texture.hpp
    thread.hpp
    transform.hpp
    uniform_info.hpp
    utils.hpp
    asset_loaders/asset_loader.hpp
    asset_loaders/fbx.hpp
    asset_loaders/png.hpp
    asset_loaders/mat.hpp
    asset_loaders/shader.hpp)

add_library(gl_lib STATIC ${LIB_HEADERS} ${LIB_SOURCES})
add_executable(gl main.cpp)
target_link_libraries(gl PUBLIC gl_lib)

target_include_directories(gl_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  gl_lib
  PUBLIC glfw
         glm
         assimp::assimp
         PNG::PNG
         profiler::lib
         spdlog::spdlog
         freetype)
target_include_directories(gl_lib PUBLIC glad/include)

add_subdirectory(tools)
