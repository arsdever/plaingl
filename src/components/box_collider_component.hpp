#pragma once

#include "components/collider_component.hpp"

class box_collider_component : public collider_component
{
public:
    box_collider_component(game_object* parent);

    void draw_gizmos() override;

    void set_position(glm::vec3 position);
    void set_rotation(glm::quat rotation);
    void set_scale(glm::vec3 scale);

    glm::vec3 get_position() const;
    glm::quat get_rotation() const;
    glm::vec3 get_scale() const;

    static constexpr std::string_view class_type_id = "box_collider_component";

protected:
    std::optional<collision>
    detect_collision(std::array<glm::vec3, 2> ray) override;

private:
    glm::vec3 _position { 0, 0, 0 };
    glm::quat _rotation { glm::identity<glm::quat>() };
    glm::vec3 _scale { 1, 1, 1 };
};
