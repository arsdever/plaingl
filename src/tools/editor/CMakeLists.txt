add_library(${PROJECT}_tools_editor STATIC editor_window.cpp editor_window.hpp)
add_library(${PROJECT}::tools::editor ALIAS ${PROJECT}_tools_editor)

target_link_libraries(${PROJECT}_tools_editor PUBLIC ${PROJECT}::lib)
target_precompile_headers(${PROJECT}_tools_editor REUSE_FROM ${PROJECT}::common)
