#pragma once

#include "experimental/project/object.hpp"

namespace components
{
class component;
class transform;
} // namespace components

class game_object
    : public object
    , public std::enable_shared_from_this<game_object>
{
public:
    static std::shared_ptr<game_object> create();

    components::transform& get_transform() const;

    bool has_parent() const;
    game_object& get_parent() const;
    void set_parent(game_object&);

    template <typename T, typename... ARGS>
        requires(std::is_base_of<components::component, T>::value)
    T& add(ARGS&&... args)
    {
        return T::create(*this, std::forward<ARGS>(args)...);
    }

    template <typename T>
        requires(std::is_base_of<components::component, T>::value)
    T& get() const
    {
        return T::get(*this);
    }

private:
    friend class memory_manager;
    game_object();

private:
    std::weak_ptr<game_object> _parent;
};
