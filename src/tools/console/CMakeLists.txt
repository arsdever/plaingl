add_library(${PROJECT}_tools_console STATIC console.cpp console.hpp)
add_library(${PROJECT}::tools::console ALIAS ${PROJECT}_tools_console)

target_precompile_headers(${PROJECT}_tools_console REUSE_FROM ${PROJECT}::common)
target_link_libraries(${PROJECT}_tools_console PUBLIC ${PROJECT}::common)
