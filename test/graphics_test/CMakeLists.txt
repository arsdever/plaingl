add_executable(${PROJECT}_texture_test texture.cpp)
target_link_libraries(${PROJECT}_texture_test PUBLIC ${PROJECT}::lib)
