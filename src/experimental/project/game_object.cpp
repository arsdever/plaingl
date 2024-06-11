#include "experimental/project/game_object.hpp"

#include "experimental/project/components/transform.hpp"
#include "experimental/project/memory_manager.hpp"

game_object::game_object() = default;

std::shared_ptr<game_object> game_object::create()
{
    auto obj = memory_manager::create_game_object();
    obj->add<components::transform>();
    obj->set_name("New game object");
    return obj;
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
