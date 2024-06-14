add_library(${PROJECT}_common OBJECT stdafx.cpp stdafx.hpp event.hpp)
add_library(${PROJECT}::common ALIAS ${PROJECT}_common)
target_precompile_headers(${PROJECT}_common PUBLIC
                          "$<$<COMPILE_LANGUAGE:CXX>:stdafx.hpp>")
target_include_directories(${PROJECT}_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT}_common PUBLIC ${PROJECT}::glad glfw glm)
