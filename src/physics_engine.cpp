#include "physics_engine.hpp"

#include "scene.hpp"

std::optional<collider_component::collision>
physics_engine::raycast(glm::vec3 from, glm::vec3 dir)
{
    if (auto* s = scene::get_active_scene())
    {
        for (auto* obj : s->objects())
        {
            auto* collider = obj->get_component<collider_component>();
            if (!collider)
            {
                continue;
            }

            auto collision = collider->detect_collision({ from, dir });
            if (collision.has_value())
            {
                return std::move(collision);
            }
        }
    }

    return std::nullopt;
}
