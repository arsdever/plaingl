macro(define_asset asset_name)
  string(TOUPPER ${asset_name} asset_name_upper)
  string(TOLOWER ${asset_name} asset_name_lower)
  string(TOUPPER ${PROJECT} project_uppercase)

  if(${PROJECT}_SUPPORT_${asset_name_upper})
    message(STATUS "${asset_name_upper} support enabled")
    add_library(${PROJECT}_asset_${asset_name_lower} STATIC
                ${asset_name_lower}.hpp ${asset_name_lower}.cpp)
    target_link_libraries(${PROJECT}_asset_${asset_name_lower}
                          PUBLIC ${PROJECT}::common)
    target_compile_definitions(
      ${PROJECT}_common
      PUBLIC ${project_uppercase}_SUPPORTS_${asset_name_upper})

    if(${ARGC} GREATER 1)
      message(STATUS "${asset_name_upper} support requires libraries ${ARGN}")
      target_link_libraries(${PROJECT}_asset_${asset_name_lower}
                            PRIVATE ${ARGN})
    endif()

    add_library(${PROJECT}::asset::${asset_name_lower} ALIAS
                ${PROJECT}_asset_${asset_name_lower})
    target_precompile_headers(${PROJECT}_asset_${asset_name_lower} REUSE_FROM
                              ${PROJECT}::common)
  endif()
endmacro()

define_asset(fbx assimp::assimp glm ${PROJECT}::graphics)
define_asset(png PNG::PNG ${PROJECT}::graphics)
define_asset(jpg JPEG::JPEG ${PROJECT}::graphics)
define_asset(mat nlohmann_json::nlohmann_json ${PROJECT}::graphics)
define_asset(shader glm ${PROJECT}::graphics)
