#pragma once

#include <array>
#include <optional>

#include <glm/vec2.hpp>

class gizmo_object
{
public:
    gizmo_object();

    void update();

    std::optional<std::array<glm::vec2, 2>> _line;
};
