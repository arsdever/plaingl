add_executable(${PROJECT} application_commands.hpp application_commands.cpp
  application.hpp application.cpp main.cpp)

target_link_libraries(
  ${PROJECT} PUBLIC ${PROJECT}::lib ${PROJECT}::tools ${PROJECT}::assets
  ${PROJECT}::scripting)
target_precompile_headers(${PROJECT} REUSE_FROM ${PROJECT}::common)

if(QT_APPS)
  message(STATUS "Configuring Qt application")
  find_package(Qt6 QUIET COMPONENTS Core Widgets Gui)

  if(Qt6_FOUND)
    add_subdirectory(qt)
  else()
    message(WARNING "Qt was not found, Qt applications will not be built")
  endif()
endif()
