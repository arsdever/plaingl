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
    void deinit();
    void draw_gizmos();

    transform& get_transform();
    const transform& get_transform() const;

    component* get_component(std::string_view type_id);

    template <typename T>
    void create_component()
    {
        _components.emplace_back(new T(this));
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

private:
    transform _transformation;
    bool _selected = false;

    std::vector<component*> _components;
};
