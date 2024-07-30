add_library(
  ${PROJECT}_common STATIC
  event.hpp
  file.cpp
  file.hpp
  logging.hpp
  stdafx.cpp
  stdafx.hpp
  utils.hpp)
add_library(${PROJECT}::common ALIAS ${PROJECT}_common)
target_precompile_headers(${PROJECT}_common PUBLIC
                          "$<$<COMPILE_LANGUAGE:CXX>:stdafx.hpp>")
target_include_directories(${PROJECT}_common PUBLIC ${PROJECT_SRC_DIR}
                                                    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  ${PROJECT}_common
  PUBLIC ${PROJECT}::glad glfw glm spdlog::spdlog
  PRIVATE filewatch)
