#pragma once

namespace graphics
{
bool initialize();

void clear(glm::dvec4 background);

void set_viewport(glm::vec2 top_left, glm::vec2 bottom_right);
} // namespace graphics
