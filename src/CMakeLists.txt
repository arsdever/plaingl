set(PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

link_libraries(profiler::lib)

add_subdirectory(common)
add_subdirectory(asset_management)
add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(project)
add_subdirectory(tools)
add_subdirectory(scripting)

add_library(${PROJECT}_lib INTERFACE)
add_library(${PROJECT}::lib ALIAS ${PROJECT}_lib)

target_precompile_headers(${PROJECT}_lib REUSE_FROM ${PROJECT}::common)
target_link_libraries(
  ${PROJECT}_lib INTERFACE ${PROJECT}::common ${PROJECT}::core
                           ${PROJECT}::graphics ${PROJECT}::project)

add_subdirectory(application)
add_subdirectory(samples)
