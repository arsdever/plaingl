#include <entt/entt.hpp>

#include "memory_manager.hpp"

#include "project/components/transform.hpp"
#include "project/scene.hpp"
#include "project/serializer_json.hpp"

memory_manager::memory_manager() = default;

memory_manager::~memory_manager() = default;

void memory_manager::initialize()
{
    if (!_instance)
    {
        _instance = new memory_manager;
    }
}

void memory_manager::deinitialize()
{
    if (_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}

memory_manager& memory_manager::instance()
{
    if (!_instance)
    {
        initialize();
    }
    return *_instance;
}

size_t memory_manager::type_id(std::string_view class_name)
{
    return entt::hashed_string(class_name.data(), class_name.size());
}

entt::meta_type memory_manager::typeof(std::string_view class_name)
{
    return entt::resolve(type_id(class_name));
}

auto& memory_manager::storage_for(std::string_view class_name)
{
    return *instance()._registry.storage(type_id(class_name));
}

std::shared_ptr<game_object> memory_manager::create_game_object()
{
    auto obj = std::shared_ptr<game_object>(new game_object);
    auto ent = instance()._registry.create();
    instance()._registry.emplace<std::shared_ptr<game_object>>(ent, obj);
    instance()._registry.emplace<uid>(ent, obj->id());
    instance()._objects.emplace(obj->id(), ent);
    return obj;
}

void memory_manager::register_component_type(std::string_view type_name)
{
    auto& _ = instance()._registry.storage<component>(type_id(type_name));
}

component& memory_manager::create_component(game_object& obj,
                                            std::string_view class_name)
{
    auto type = typeof(class_name);
    auto* storage = instance()._registry.storage(type.id());
    auto comp = type.construct(entt::forward_as_meta(obj));
    auto base = type.base();
    auto ent = entity(obj.id());
    if (storage->contains(ent))
    {
        storage->remove(ent);
    }
    storage->push(entity(obj.id()), comp.data());

    return get_component(obj, class_name);
}

component& memory_manager::get_component(const game_object& obj,
                                         std::string_view class_name)
{
    return *try_get_component(obj, class_name);
}

component* memory_manager::try_get_component(const game_object& obj,
                                             std::string_view class_name)
{
    auto& storage = storage_for(class_name);
    if (!storage.contains(entity(obj.id())))
    {
        return nullptr;
    }

    return static_cast<component*>(storage.value(entity(obj.id())));
}

std::shared_ptr<game_object> memory_manager::get_object(entt::entity ent)
{
    return instance()._registry.get<std::shared_ptr<game_object>>(ent);
}

std::shared_ptr<game_object> memory_manager::get_object_by_id(uid id)
{
    return get_object(entity(id));
}

bool memory_manager::visit_components(const game_object& obj,
                                      std::function<bool(component&)> visitor)
{
    auto ent = entity(obj.id());
    for (auto [ _, storage ] : instance()._registry.storage())
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
    for (auto ent : instance()._registry.view<entt::entity>())
    {
        auto obj = get_object(ent);
        s.start_object();
        s.property("name", obj->get_name());
        s.property("id", obj->id().id);
        s.property("is_active", obj->is_active());
        if (auto p = obj->get_parent(); p != nullptr)
        {
            s.property("parent", p->id().id);
        }
        for (auto [ _, storage ] : instance()._registry.storage())
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

        auto n = instance()._objects.extract(old_id);
        n.key() = gobj->id();
        instance()._objects.insert(std::move(n));
        auto ent = entity(gobj->id());

        if (obj.contains("parent"))
        {
            // implement this
        }
        else
        {
            scene::_active_scene->add_root_object(gobj);
        }
        instance()._registry.replace<std::shared_ptr<game_object>>(ent, gobj);
        instance()._registry.replace<uid>(ent, gobj->id());

        for (const auto& c : obj[ "components" ])
        {
            auto type = entt::resolve(c[ "type" ]);
            auto comp = type.construct(entt::forward_as_meta(*gobj));
            type.invoke(entt::hashed_string("deserialize"),
                        comp,
                        entt::forward_as_meta(c));
            for (auto&& curr : instance()._registry.storage())
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

uid memory_manager::id(entt::entity ent)
{
    return instance()._registry.get<uid>(ent);
}

entt::entity memory_manager::entity(uid id)
{
    return instance()._objects.at(id);
}

memory_manager* memory_manager::_instance = nullptr;
