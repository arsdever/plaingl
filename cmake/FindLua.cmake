if(NOT DEFINED ENV{LUA_DIR})
  message(
    FATAL_ERROR
      "Lua directory is not set.\
      Check if Lua is installed and set the LUA_DIR environment variable to the location of the Lua directory.\
      You can download Lua binaries from https://luabinaries.sourceforge.net/")
endif()
add_library(lua54 INTERFACE)
target_link_libraries(lua54 INTERFACE lua54.lib)
target_include_directories(lua54 INTERFACE "$ENV{LUA_DIR}/include")
target_link_directories(lua54 INTERFACE "$ENV{LUA_DIR}")
