#include "project/components/component_registry.hpp"

#include "component_list_inc.hpp"
#include "project/project_manager.hpp"
#include "project/serializer_json.hpp"

void component_registry::register_components()
{
    for (auto& [ name, func ] : _declaration_table)
        func();
}
