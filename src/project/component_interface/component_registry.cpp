#include "project/component_interface/component_registry.hpp"

std::unordered_map<std::string,
                   std::function<std::shared_ptr<component>(game_object&)>,
                   string_hash,
                   std::equal_to<>>
    component_registry::_type_map;

std::unordered_map<size_t, metatype> component_registry::_types;
