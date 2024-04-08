#pragma once

#include "components/collider_component.hpp"

class physics_engine
{
public:
    std::optional<collider_component::collision> raycast(glm::vec3 from,
                                                         glm::vec3 dir);
};
