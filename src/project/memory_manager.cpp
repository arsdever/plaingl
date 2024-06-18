#include <entt/entt.hpp>

#include "memory_manager.hpp"

#include "project/components/component_registry.hpp"
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

std::shared_ptr<game_object> memory_manager::get_object(entt::entity ent)
{
    return instance()._registry.get<std::shared_ptr<game_object>>(ent);
}

std::shared_ptr<game_object> memory_manager::get_object_by_id(uid id)
{
    return get_object(entity(id));
}

bool memory_manager::visit_components(
    const game_object& obj, std::function<bool(components::component&)> visitor)
{
    auto ent = entity(obj.id());
    for (auto [ _, storage ] : instance()._registry.storage())
    {
        if (storage.contains(ent))
        {
            entt::meta_type type = entt::resolve(storage.type());
            if (auto component = type.from_void(storage.value(ent)))
            {
                if (auto c = component.try_cast<components::component>())
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
            auto comp = type.construct();
            type.invoke(entt::hashed_string("deserialize"),
                        comp,
                        entt::forward_as_meta(c));
            _component_creators.at(type.info().hash())(*gobj);
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

std::unordered_map<entt::id_type, std::function<void(game_object&)>>
    memory_manager::_component_creators;
