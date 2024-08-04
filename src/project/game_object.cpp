#include "project/game_object.hpp"

#include "project/component.hpp"
#include "project/components/transform.hpp"
#include "project/project_manager.hpp"

game_object::game_object() = default;

std::shared_ptr<game_object> game_object::create()
{
    auto obj = project_manager::create_game_object();
    obj->add<components::transform>();
    obj->set_name("New game object");
    return obj;
}

component& game_object::get(std::string_view class_name) const
{
    return project_manager::get_component(*this, class_name);
}

component* game_object::try_get(std::string_view class_name) const
{
    return project_manager::try_get_component(*this, class_name);
}

components::transform& game_object::get_transform() const
{
    return get<components::transform>();
}

bool game_object::has_parent() const { return _parent.lock() != nullptr; }

std::shared_ptr<game_object> game_object::get_parent() const
{
    return _parent.lock();
}

void game_object::set_parent(std::shared_ptr<game_object> obj)
{
    _parent = obj;
}

std::shared_ptr<game_object> game_object::get_child_at(size_t index) const
{
    if (index >= _children.size())
        return nullptr;
    return _children[ index ];
}

std::shared_ptr<game_object>
game_object::get_child_by_name(std::string_view name) const
{
    std::shared_ptr<game_object> result;
    visit_children(
        [ & ](const std::shared_ptr<game_object>& obj)
    {
        if (obj->get_name() == name)
        {
            result = obj;
            return false;
        }
        return true;
    });
    return result;
}

void game_object::add_child(std::shared_ptr<game_object> child)
{
    _children.push_back(child);
    if (child->get_parent() != child)
        child->set_parent(shared_from_this());
}

void game_object::remove_child(std::shared_ptr<game_object> child)
{
    auto rm = std::remove(_children.begin(), _children.end(), child);
    std::for_each(
        rm, _children.end(), [](auto& obj) { obj->set_parent(nullptr); });
    _children.erase(rm, _children.end());
}

bool game_object::visit_children(
    std::function<bool(const std::shared_ptr<game_object>&)> visitor) const
{
    for (auto& child : _children)
    {
        if (!visitor(child))
            return false;
    }
    return true;
}

void game_object::visit_components(
    std::function<bool(component&)> visitor) const
{
    project_manager::visit_components(*this,
                                      [ & ](auto& c)
    {
        if (visitor(c))
            return true;
        return false;
    });
}

void game_object::set_active(bool active) { _is_active = active; }

bool game_object::is_active() const { return _is_active; }

void game_object::init()
{
    if (!is_active())
    {
        return;
    }

    visit_components(
        [](auto& c) -> bool
    {
        c.init();
        return true;
    });
}

void game_object::update()
{
    if (!is_active())
    {
        return;
    }

    visit_components(
        [](auto& c) -> bool
    {
        c.update();
        return true;
    });
}

void game_object::draw_gizmos()
{
    if (!is_active())
    {
        return;
    }

    visit_components(
        [](auto& c) -> bool
    {
        // c.draw_gizmos();
        return true;
    });
}

void game_object::deinit()
{
    if (!is_active())
    {
        return;
    }

    visit_components(
        [](auto& c) -> bool
    {
        c.deinit();
        return true;
    });
}

component& game_object::add(std::string_view class_name)
{
    return project_manager::instance().create_component(*this, class_name);
}
