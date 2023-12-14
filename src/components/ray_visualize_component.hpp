#pragma once

#include <glm/vec3.hpp>

#include "component.hpp"

class ray_visualize_component : public component
{
public:
    ray_visualize_component(game_object* parent);

    void set_ray(glm::vec3 from, glm::vec3 direction);

    void draw_gizmos() override;

    static constexpr std::string_view class_type_id = "ray_visualize_component";

private:
    glm::vec3 _ray_from {};
    glm::vec3 _ray_direction {};
};
