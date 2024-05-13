add_library(${PROJECT}_glad STATIC src/gl.c)
add_library(${PROJECT}::glad ALIAS ${PROJECT}_glad)

target_include_directories(${PROJECT}_glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
