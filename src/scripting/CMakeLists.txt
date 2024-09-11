add_library(${PROJECT}_scripting STATIC backend.hpp backend.cpp script.hpp
                                        script.cpp scripting_fwd.hpp)
add_library(${PROJECT}::scripting ALIAS ${PROJECT}_scripting)

target_precompile_headers(${PROJECT}_scripting REUSE_FROM ${PROJECT}::common)

target_link_libraries(${PROJECT}_scripting PUBLIC ${PROJECT}::common)

add_subdirectory(python)

target_link_libraries(${PROJECT}_scripting
                      PRIVATE ${PROJECT}::scripting::python)
