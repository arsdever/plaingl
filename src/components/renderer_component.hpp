#pragma once

#include "component.hpp"

class material;

class renderer_component : public component
{
public:
    inline renderer_component(game_object* parent,
                              std::string_view type_id = class_type_id)
        : component(parent, type_id)
    {
    }

    inline material* get_material() const { return _material; }

    inline void set_material(material* mat) { _material = mat; }

    virtual void render() = 0;

    static constexpr std::string_view class_type_id = "renderer_component";

protected:
    material* _material = nullptr;
};
