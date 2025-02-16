#pragma once

namespace graphics
{
using graphics_procedure = void (*)();
using graphics_proc_address_getter = graphics_procedure (*)(const char*);

bool initialize(graphics_proc_address_getter proc_getter);

void clear(glm::dvec4 background);

void set_viewport(glm::vec2 top_left, glm::vec2 bottom_right);

void set_wireframe(bool value);
} // namespace graphics
