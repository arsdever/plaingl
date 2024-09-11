#include <entt/entt.hpp>

#include "project/components/component_registry.hpp"

#include "component_list_inc.hpp"
#include "project/memory_manager.hpp"
#include "project/serializer_json.hpp"

void component_registry::register_components()
{
    for (auto& [ name, func ] : _declaration_table)
        func();
}

void component_registry::register_component(std::string_view name,
                                            std::function<size_t()> func)
{
    _declaration_table[ name ] = func;
}

std::unordered_map<std::string_view, std::function<size_t()>>
    component_registry::_declaration_table {
#include "component_list.hpp"
    };
