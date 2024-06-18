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

void component::set_active(bool active) { _is_active = active; }

bool component::is_active() const { return _is_active; }

void component::init()
{
    if (!is_active())
        return;
    on_init();
}

void component::update()
{
    if (!is_active())
        return;
    on_update();
}

void component::deinit()
{
    if (!is_active())
        return;
    on_deinit();
}

void component::on_init() { }

void component::on_update() { }

void component::on_deinit() { }
} // namespace components
