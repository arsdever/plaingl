#pragma once

#include "graphics/graphics_fwd.hpp"
#include "graphics/renderer/renderer.hpp"

class renderer_2d : public renderer
{
public:
    void draw_rect(glm::vec2 top_left,
                   glm::vec2 bottom_right,
                   glm::vec2 window_size,
                   std::shared_ptr<texture> texture);

    void draw_rect(glm::vec2 top_left,
                   glm::vec2 bottom_right,
                   glm::vec2 window_size,
                   float border_thickness,
                   glm::vec4 border_color,
                   glm::vec4 fill_color);

    void draw_text(glm::vec2 baseline,
                   const font& f,
                   const glm::vec2& window_size,
                   std::string_view text,
                   float scale = 1.0f);
};
