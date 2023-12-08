#pragma once

#include "material.hpp"
#include "mesh.hpp"
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
        return static_cast<T*>(get_component(T::class_type_id));
    }

private:
    transform _transformation;
    bool _selected = false;

    std::vector<component*> _components;
};
