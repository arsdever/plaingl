#include <entt/entt.hpp>

#include "project/components/component.hpp"

#include "project/components/transform.hpp"
#include "project/game_object.hpp"

namespace components
{
component::component(const std::string& name, game_object& obj)
    : _transform(obj.get<transform>())
    , _game_object(obj)
{
    set_name(name);
}

size_t component::register_component()
{
    static constexpr auto id = entt::hashed_string("component");
    entt::meta<component>().type(id);
    return id;
}

void component::on_start() { }

void component::on_update() { }

void component::on_destroy() { }
} // namespace components
