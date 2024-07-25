#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <entt/meta/utility.hpp>
#include <nlohmann/json.hpp>

#include "memory_manager.hpp"

#include "project/components/transform.hpp"
#include "project/game_object.hpp"
#include "project/scene.hpp"
#include "project/serializer_json.hpp"

struct memory_manager::impl
{
    entt::registry _registry;
    std::unordered_map<uid, entt::entity> _entities;

    inline auto& storage_for(std::string_view class_name)
    {
        return *_registry.storage(type_id(class_name));
    }

    static entt::meta_type typeof(std::string_view class_name)
    {
        return entt::resolve(type_id(class_name));
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
};

memory_manager::memory_manager()
    : _impl(std::make_unique<impl>())
{
}

memory_manager::~memory_manager() = default;

memory_manager& memory_manager::instance()
{
    static memory_manager instance;
    return instance;
}

size_t memory_manager::type_id(std::string_view class_name)
{
    return entt::hashed_string(class_name.data(), class_name.size());
}

std::shared_ptr<game_object> memory_manager::create_game_object()
{
    auto obj = std::shared_ptr<game_object>(new game_object);
    auto ent = instance()._impl->_registry.create();
    instance()._impl->_registry.emplace<std::shared_ptr<game_object>>(ent, obj);
    instance()._impl->_registry.emplace<uid>(ent, obj->id());
    instance()._impl->_entities.emplace(obj->id(), ent);
    instance()._objects.emplace(obj->id(), obj);
    return obj;
}

void memory_manager::register_component_type(std::string_view type_name)
{
    auto& _ =
        instance()._impl->_registry.storage<component>(type_id(type_name));
}

void memory_manager::for_each_object(
    std::function<void(std::shared_ptr<object>&)> func)
{
    for (auto& [ _, obj ] : instance()._objects)
    {
        if (auto sobj = obj.lock(); sobj != nullptr)
            func(sobj);
    }
}

component& memory_manager::create_component(game_object& obj,
                                            std::string_view class_name)
{
    auto type = impl::typeof(class_name);
    auto comp =
        type.construct(entt::forward_as_meta(instance()._impl->_registry),
                       instance()._impl->entity(obj.id()),
                       entt::forward_as_meta(obj));
    return *(static_cast<component*>(comp.data()));
}

component& memory_manager::get_component(const game_object& obj,
                                         std::string_view class_name)
{
    return *try_get_component(obj, class_name);
}

component* memory_manager::try_get_component(const game_object& obj,
                                             std::string_view class_name)
{
    auto type = impl::typeof(class_name);
    auto comp = type.invoke(entt::hashed_string("try_get"),
                            {},
                            entt::forward_as_meta(instance()._impl->_registry),
                            instance()._impl->entity(obj.id()));
    return (*comp).try_cast<component>();
}

std::shared_ptr<object> memory_manager::get_object_by_id(uid id)
{
    return instance()._objects.at(id).lock();
}

std::shared_ptr<game_object> memory_manager::get_game_object(uid id)
{
    return instance()._impl->get_object(instance()._impl->entity(id));
}

bool memory_manager::visit_components(const game_object& obj,
                                      std::function<bool(component&)> visitor)
{
    auto ent = instance()._impl->entity(obj.id());
    for (auto [ _, storage ] : instance()._impl->_registry.storage())
    {
        if (storage.contains(ent))
        {
            entt::meta_type type = entt::resolve(storage.type());
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

nlohmann::json memory_manager::serialize()
{
    json_serializer s;
    for (auto ent : instance()._impl->_registry.view<entt::entity>())
    {
        auto obj = instance()._impl->get_object(ent);
        s.start_object();
        s.property("name", obj->get_name());
        s.property("id", obj->id().id);
        s.property("is_active", obj->is_active());
        if (auto p = obj->get_parent(); p != nullptr)
        {
            s.property("parent", p->id().id);
        }
        for (auto [ _, storage ] : instance()._impl->_registry.storage())
        {
            if (storage.contains(ent))
            {
                entt::meta_type type = entt::resolve(storage.type());
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

void memory_manager::deserialize(const nlohmann::json& data)
{
    for (const auto& obj : data[ "objects" ])
    {
        auto gobj = create_game_object();
        auto old_id = gobj->id();
        gobj->set_name(obj[ "name" ]);
        gobj->_id = uid { obj[ "id" ] };
        gobj->_is_active = obj[ "is_active" ].get<bool>();

        auto n = instance()._impl->_entities.extract(old_id);
        n.key() = gobj->id();
        instance()._impl->_entities.insert(std::move(n));
        auto ent = instance()._impl->entity(gobj->id());

        if (obj.contains("parent"))
        {
            // implement this
        }
        else
        {
            scene::_active_scene->add_root_object(gobj);
        }
        instance()._impl->_registry.replace<std::shared_ptr<game_object>>(ent,
                                                                          gobj);
        instance()._impl->_registry.replace<uid>(ent, gobj->id());

        for (const auto& c : obj[ "components" ])
        {
            auto type = entt::resolve(c[ "type" ]);
            auto comp = type.construct(entt::forward_as_meta(*gobj));
            type.invoke(entt::hashed_string("deserialize"),
                        comp,
                        entt::forward_as_meta(c));
            for (auto&& curr : instance()._impl->_registry.storage())
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
