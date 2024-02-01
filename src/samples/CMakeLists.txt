add_executable(${PROJECT}_window_sample window.cpp)
target_link_libraries(${PROJECT}_window_sample PUBLIC ${PROJECT}::lib)

add_executable(${PROJECT}_viewport_sample viewport.cpp)
target_link_libraries(${PROJECT}_viewport_sample PUBLIC ${PROJECT}::lib)
