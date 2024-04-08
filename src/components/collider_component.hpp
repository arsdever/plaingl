#pragma once

#include <array>
#include <optional>

#include "component.hpp"

class collider_component : public component
{
public:
    struct collision
    {
        glm::vec3 hit_point;
        glm::vec3 hit_normal;
    };

public:
    collider_component(game_object* parent,
                       std::string_view type_id = class_type_id)
        : component(parent, type_id)
    {
    }

    static constexpr std::string_view class_type_id = "collider_component";

protected:
    friend class physics_engine;

    virtual std::optional<collision>
    detect_collision(std::array<glm::vec3, 2> ray) = 0;
};
