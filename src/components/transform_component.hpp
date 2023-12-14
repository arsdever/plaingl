#pragma once

#include <glm/ext.hpp>

#include "component.hpp"

class transform_component : public component
{
public:
    transform_component(game_object* parent);

    void draw_gizmos() override;

    void set_position(glm::vec3 position);
    void set_rotation(glm::quat rotation);
    void set_scale(glm::vec3 scale);

    glm::vec3 get_position() const;
    glm::quat get_rotation() const;
    glm::vec3 get_scale() const;

    glm::mat4 get_matrix() const;

    static constexpr std::string_view class_type_id = "transform_component";
};
