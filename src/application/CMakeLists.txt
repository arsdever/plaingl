add_executable(${PROJECT} application.hpp application.cpp main.cpp)

target_link_libraries(${PROJECT} PUBLIC ${PROJECT}::lib ${PROJECT}::tools)
target_precompile_headers(${PROJECT} REUSE_FROM ${PROJECT}::common)
