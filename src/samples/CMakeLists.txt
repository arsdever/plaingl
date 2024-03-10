add_executable(${PROJECT}_window_sample window.cpp)
target_link_libraries(${PROJECT}_window_sample PUBLIC ${PROJECT}::lib)

add_executable(${PROJECT}_viewport_sample viewport.cpp)
target_link_libraries(${PROJECT}_viewport_sample PUBLIC ${PROJECT}::lib)

add_executable(${PROJECT}_console_sample console.cpp)
target_link_libraries(${PROJECT}_console_sample PUBLIC ${PROJECT}::lib)

add_executable(${PROJECT}_multiple_viewport_sample multiple_viewports.cpp)
target_link_libraries(${PROJECT}_multiple_viewport_sample PUBLIC ${PROJECT}::lib)
