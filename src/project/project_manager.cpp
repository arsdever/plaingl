#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <entt/meta/utility.hpp>
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
    entt::registry _registry;
    std::unordered_map<uid, std::weak_ptr<object>> _objects;
    std::unordered_map<uid, entt::entity> _entities;

    inline auto& storage_for(std::string_view class_name)
    {
        return *_registry.storage(type_id(class_name));
    }

    static entt::meta_type typeof(size_t type_identifier)
    {
        return entt::resolve(component_registry::ctx, type_identifier);
    }

    static entt::meta_type typeof(std::string_view class_name)
    {
        log()->trace("Looking up for type {}", class_name);
        auto tid = type_id(class_name);
        auto t = typeof(tid);
        if (t)
        {
            log()->trace("Found type {} (id {})", class_name, tid);
            return t;
        }

        return {};
    }

    static entt::meta_type typeof(entt::type_info ti)
    {
        return typeof(ti.index());
    }

    template <typename T>
    static entt::meta_type typeof()
    {
        return typeof(T::type_name);
    }

    entt::entity entity(uid id) { return _entities.at(id); }

    uid id(entt::entity ent) { return _registry.get<uid>(ent); }

    std::shared_ptr<game_object> get_object(entt::entity ent)
    {
        return _registry.get<std::shared_ptr<game_object>>(ent);
    }

    template <typename T, typename F>
    static void register_component_type(std::string_view type_name)
    {
        component_registry::register_component<component, F>(
            component_registry::ctx, type_name);
    }
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
    return entt::hashed_string(class_name.data(), class_name.size());
}

std::shared_ptr<game_object> project_manager::create_game_object()
{
    auto obj = std::shared_ptr<game_object>(new game_object);
    auto ent = _instance->_impl->_registry.create();
    _instance->_impl->_registry.emplace<std::shared_ptr<game_object>>(ent, obj);
    _instance->_impl->_registry.emplace<uid>(ent, obj->id());
    _instance->_impl->_entities.emplace(obj->id(), ent);
    _instance->_impl->_objects.emplace(obj->id(), obj);
    return obj;
}

void project_manager::for_each_object(
    std::function<void(std::shared_ptr<object>&)> func)
{
    for (auto& [ _, obj ] : _instance->_impl->_objects)
    {
        if (auto sobj = obj.lock(); sobj != nullptr)
            func(sobj);
    }
}

component& project_manager::create_component(game_object& obj,
                                             std::string_view class_name)
{
    auto type = impl::typeof(class_name);
    auto comp =
        type.construct(entt::forward_as_meta(_instance->_impl->_registry),
                       class_name,
                       _instance->_impl->entity(obj.id()),
                       entt::forward_as_meta(obj));
    return *(static_cast<component*>(comp.data()));
}

component& project_manager::get_component(const game_object& obj,
                                          std::string_view class_name)
{
    return *try_get_component(obj, class_name);
}

component* project_manager::try_get_component(const game_object& obj,
                                              std::string_view class_name)
{
    auto entity = _instance->_impl->entity(obj.id());
    // try to lookup by base class
    for (auto&& storage_info : _instance->_impl->_registry.storage())
    {
        entt::meta_type type =
            entt::resolve(component_registry::ctx, storage_info.second.type());
        if (type.can_cast(impl::typeof(class_name)))
        {
            if (auto& storage = storage_info.second; storage.contains(entity))
            {
                return static_cast<component*>(storage.value(entity));
            }
        }
    }

    return nullptr;
}

std::shared_ptr<object> project_manager::get_object_by_id(uid id)
{
    return _instance->_impl->_objects.at(id).lock();
}

std::shared_ptr<game_object> project_manager::get_game_object(uid id)
{
    return _instance->_impl->get_object(_instance->_impl->entity(id));
}

bool project_manager::visit_components(const game_object& obj,
                                       std::function<bool(component&)> visitor)
{
    auto ent = _instance->_impl->entity(obj.id());
    for (auto [ name, storage ] : _instance->_impl->_registry.storage())
    {
        if (storage.contains(ent))
        {
            entt::meta_type type = impl::typeof(name);
            if (auto meta_component = type.from_void(storage.value(ent)))
            {
                if (auto c = meta_component.try_cast<component>())
                {
                    if (!visitor(*c))
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

nlohmann::json project_manager::serialize()
{
    json_serializer s;
    for (auto ent : _instance->_impl->_registry.view<entt::entity>())
    {
        auto obj = _instance->_impl->get_object(ent);
        s.start_object();
        s.property("name", obj->get_name());
        s.property("id", obj->id().id);
        s.property("is_active", obj->is_active());
        if (auto p = obj->get_parent(); p != nullptr)
        {
            s.property("parent", p->id().id);
        }
        for (auto [ _, storage ] : _instance->_impl->_registry.storage())
        {
            if (storage.contains(ent))
            {
                entt::meta_type type = impl::typeof(storage.type());
                auto serializer = type.func(entt::hashed_string("serialize"));
                if (serializer)
                {
                    entt::meta_any component =
                        type.from_void(storage.value(ent));
                    serializer.invoke(component, entt::forward_as_meta(s));
                }
            }
        }
        s.finish_object();
    }
    return s.root();
}

void project_manager::deserialize(const nlohmann::json& data)
{
    for (const auto& obj : data[ "objects" ])
    {
        auto gobj = create_game_object();
        auto old_id = gobj->id();
        gobj->set_name(obj[ "name" ]);
        gobj->_id = uid { obj[ "id" ] };
        gobj->_is_active = obj[ "is_active" ].get<bool>();

        auto n = _instance->_impl->_entities.extract(old_id);
        n.key() = gobj->id();
        _instance->_impl->_entities.insert(std::move(n));
        auto ent = _instance->_impl->entity(gobj->id());

        if (obj.contains("parent"))
        {
            // implement this
        }
        else
        {
            scene::_active_scene->add_root_object(gobj);
        }
        _instance->_impl->_registry.replace<std::shared_ptr<game_object>>(ent,
                                                                          gobj);
        _instance->_impl->_registry.replace<uid>(ent, gobj->id());

        for (const auto& c : obj[ "components" ])
        {
            auto type = entt::resolve(c[ "type" ]);
            auto comp = type.construct(entt::forward_as_meta(*gobj));
            type.invoke(entt::hashed_string("deserialize"),
                        comp,
                        entt::forward_as_meta(c));
            for (auto&& curr : _instance->_impl->_registry.storage())
            {
                if (auto& storage = curr.second; storage.contains(ent))
                {
                    if (storage.type() == type.info())
                    {
                        storage.push(ent, comp.data());
                    }
                }
            }
        }
    }
}

std::unique_ptr<project_manager> project_manager::_instance { nullptr };
