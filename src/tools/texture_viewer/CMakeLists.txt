add_library(${PROJECT}_tools_texture_viewer STATIC texture_viewer.hpp
                                                   texture_viewer.cpp)
add_library(${PROJECT}::tools::texture_viewer ALIAS
            ${PROJECT}_tools_texture_viewer)

target_link_libraries(${PROJECT}_tools_texture_viewer
                      PUBLIC ${PROJECT}::lib ${PROJECT}::graphics)
target_precompile_headers(${PROJECT}_tools_texture_viewer REUSE_FROM
                          ${PROJECT}::common)
