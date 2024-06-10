#include "experimental/project/components/component.hpp"

#include "experimental/project/components/transform.hpp"
#include "experimental/project/game_object.hpp"

namespace components
{
component::component(const std::string& name, game_object& obj)
    : _transform(obj.get<transform>())
    , _game_object(obj)
{
    set_name(name);
}

void component::on_start() { }

void component::on_update() { }

void component::on_destroy() { }
} // namespace components
