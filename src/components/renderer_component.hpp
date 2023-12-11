#pragma once

#include "component.hpp"

class renderer_component : public component
{
public:
    inline renderer_component(game_object* parent,
                              std::string_view type_id = class_type_id)
        : component(parent, type_id)
    {
    }

    virtual void render() = 0;

    static constexpr std::string_view class_type_id = "renderer_component";
};
