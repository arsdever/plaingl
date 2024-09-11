add_executable(${PROJECT} application_commands.hpp application_commands.cpp
                          application.hpp application.cpp main.cpp)

target_link_libraries(
  ${PROJECT} PUBLIC ${PROJECT}::lib ${PROJECT}::tools ${PROJECT}::assets
                    ${PROJECT}::scripting)
target_precompile_headers(${PROJECT} REUSE_FROM ${PROJECT}::common)
