find_package(pybind11 CONFIG REQUIRED)

add_library(${PROJECT}_scripting_python python_backend.hpp python_backend.cpp)
# project_bindings.cpp project_bindings.hpp)
add_library(${PROJECT}::scripting::python ALIAS ${PROJECT}_scripting_python)

target_link_libraries(${PROJECT}_scripting_python
                      PUBLIC ${PROJECT}::project pybind11::embed EnTT::EnTT)
target_precompile_headers(${PROJECT}_scripting_python REUSE_FROM
                          ${PROJECT}::common)
