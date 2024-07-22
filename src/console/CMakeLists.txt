add_library(${PROJECT}_console STATIC console.cpp console.hpp)
add_library(${PROJECT}::console ALIAS ${PROJECT}_console)

target_precompile_headers(${PROJECT}_console REUSE_FROM ${PROJECT}::common)
target_link_libraries(${PROJECT}_console PUBLIC ${PROJECT}::common)
