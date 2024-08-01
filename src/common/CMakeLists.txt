add_library(
  ${PROJECT}_common STATIC
  event.hpp
  file_watcher.cpp
  file_watcher.hpp
  file.cpp
  file.hpp
  filesystem.cpp
  filesystem.hpp
  logging.hpp
  stdafx.cpp
  stdafx.hpp
  utils.hpp)
add_library(${PROJECT}::common ALIAS ${PROJECT}_common)
target_precompile_headers(${PROJECT}_common PUBLIC
                          "$<$<COMPILE_LANGUAGE:CXX>:stdafx.hpp>")
target_include_directories(${PROJECT}_common PUBLIC ${PROJECT_SRC_DIR}
                                                    ${CMAKE_CURRENT_SOURCE_DIR})
if(${USE_FILEWATCH})
  message(STATUS "Using \"filewatch\" library for file watcher")
  target_compile_definitions(${PROJECT}_common PRIVATE "-DUSE_FILEWATCH=1")
  target_sources(${PROJECT}_common PRIVATE impl/file_watcher_filewatch.hpp)
  target_link_libraries(${PROJECT}_common PRIVATE filewatch)
endif()

if(WIN32)
  target_sources(${PROJECT}_common PRIVATE impl/file_win.hpp)
  target_link_libraries(${PROJECT}_common PRIVATE kernel32)
endif()

target_link_libraries(${PROJECT}_common PUBLIC ${PROJECT}::glad glfw glm
                                               spdlog::spdlog)
