#include <nlohmann/json.hpp>

#include "project_manager.hpp"

#include "common/logging.hpp"
#include "component_interface/component_registry.hpp"
#include "project/components/camera.hpp"
#include "project/components/light.hpp"
#include "project/components/mesh_filter.hpp"
#include "project/components/mesh_renderer.hpp"
#include "project/components/transform.hpp"
#include "project/game_object.hpp"
#include "project/scene.hpp"
#include "project/serializer_json.hpp"

namespace
{
logger log() { return get_logger("project_manager"); }
}; // namespace

struct project_manager::impl
{
    std::unordered_map<uid, std::shared_ptr<object>> _objects;
    std::vector<std::shared_ptr<game_object>> _game_objects;
    std::vector<std::shared_ptr<component>> _components;
    std::unordered_multimap<uid, std::shared_ptr<component>>
        _game_object_component_mapping;
};

project_manager::project_manager()
    : _impl(std::make_unique<impl>())
{
}

project_manager::~project_manager() = default;

void project_manager::initialize()
{
    _instance = std::unique_ptr<project_manager>(new project_manager());
    component_registry::register_components<components::camera,
                                            components::light,
                                            components::mesh_filter,
                                            components::mesh_renderer,
                                            components::transform>();
}

void project_manager::shutdown() { _instance = nullptr; }

size_t project_manager::type_id(std::string_view class_name)
{
    return component_registry::type_id(class_name);
}

std::shared_ptr<game_object> project_manager::create_game_object()
{
    auto obj = std::shared_ptr<game_object>(new game_object);
    _instance->_impl->_objects.emplace(obj->id(), obj);
    _instance->_impl->_game_objects.push_back(obj);
    return obj;
}

component& project_manager::create_component(game_object& obj,
                                             std::string_view class_name)
{
    auto comp = component_registry::instantiate(class_name, obj);
    _instance->_impl->_objects.emplace(comp->id(), comp);
    _instance->_impl->_components.push_back(comp);
    _instance->_impl->_game_object_component_mapping.emplace(obj.id(), comp);
    return *comp;
}

component& project_manager::get_component(const game_object& obj,
                                          std::string_view class_name)
{
    return *try_get_component(obj, class_name);
}

std::shared_ptr<component>
project_manager::try_get_component(const game_object& obj,
                                   std::string_view class_name)
{
    std::shared_ptr<component> result;

    visit_components(obj,
                     [ &result, &class_name ](std::shared_ptr<component> comp)
    {
        if (comp->can_cast(component_registry::get_type(class_name)))
        {
            result = comp;
            return false;
        }
        return true;
    });

    return result;
}

std::shared_ptr<object> project_manager::try_get_object(uid id)
{
    auto it = _instance->_impl->_objects.find(id);

    if (it == _instance->_impl->_objects.end())
        return nullptr;

    return it->second;
}

nlohmann::json project_manager::serialize()
{
    throw std::exception("Not implemented");
}

void project_manager::deserialize(const nlohmann::json& data)
{
    throw std::exception("Not implemented");
}

bool project_manager::visit_objects(
    std::function<bool(std::shared_ptr<object>)> visitor)
{
    for (auto& [ _, obj ] : _instance->_impl->_objects)
    {
        if (!visitor(obj))
        {
            return false;
        }
    }

    return true;
}

bool project_manager::visit_game_objects(
    std::function<bool(std::shared_ptr<game_object>)> visitor)
{
    for (auto& obj : _instance->_impl->_game_objects)
    {
        if (!visitor(obj))
        {
            return false;
        }
    }

    return true;
}

bool project_manager::visit_components(
    const game_object& obj,
    std::function<bool(std::shared_ptr<component>)> visitor)
{
    auto& map = _instance->_impl->_game_object_component_mapping;
    auto bucket = map.bucket(obj.id());

    for (auto it = map.begin(bucket); it != map.end(bucket); ++it)
    {
        if (!visitor(it->second))
        {
            return false;
        }
    }

    return true;
}

std::unique_ptr<project_manager> project_manager::_instance { nullptr };
