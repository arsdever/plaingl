#pragma once

#include "material.hpp"
#include "mesh.hpp"
#include "transform.hpp"

class renderer;

class game_object
{
public:
    game_object();

    void set_selected(bool selected = true);
    bool is_selected() const;

    void update();

    transform& get_transform();
    const transform& get_transform() const;

    // TODO: change when component interface is ready
    template <typename T>
    T* get_component() const;

private:
    transform _transformation;
    renderer* _mesh_renderer;
    bool _selected = false;
};
