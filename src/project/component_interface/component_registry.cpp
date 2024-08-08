#include "project/component_interface/component_registry.hpp"

#include "project/project_manager.hpp"
#include "project/serializer_json.hpp"

// void component_registry::register_components()
// {
//     for (auto& [ name, func ] : _declaration_table)
//         func();
// }

entt::meta_ctx component_registry::ctx;
