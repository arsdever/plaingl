#pragma once

#include <string_view>
#include <vector>

#include "transform.hpp"

class component;

class game_object
{
public:
    game_object();

    void set_selected(bool selected = true);
    bool is_selected() const;

    void init();
    void update();
    void draw_gizmos();
    void deinit();

    void set_active(bool active_flag = true);
    bool is_active() const;

    void set_name(std::string_view name);
    std::string_view get_name() const;

    transform& get_transform();
    const transform& get_transform() const;

    component* get_component(std::string_view type_id);

    template <typename T>
    T* create_component()
    {
        return static_cast<T*>(_components.emplace_back(new T(this)));
    }

    template <typename T>
    T* get_component()
    {
        // TODO: need to figure out a cleverer way of checking the component
        // hierarchy to find matching component
        auto iterator = std::find_if(_components.begin(),
                                     _components.end(),
                                     [](auto* component) -> bool
        { return dynamic_cast<T*>(component); });

        return iterator == _components.end() ? nullptr
                                             : static_cast<T*>(*iterator);
    }

    void add_child(game_object* child);
    std::vector<game_object*>& get_children();
    void set_parent(game_object* parent);
    game_object* get_parent();

private:
    transform _transformation;
    bool _selected = false;
    bool _is_active = true;
    std::string _name;

    std::vector<component*> _components;
    game_object* _parent;
    std::vector<game_object*> _children;
};
