#pragma once

#include <glm/vec2.hpp>

class viewport
{
public:
    void init();
    void set_active();

    size_t width() const;
    size_t height() const;
    glm::uvec2 resolution() const;
    void resize(size_t width, size_t height);
    void resize(glm::uvec2 size);

private:
    glm::uvec2 _resolution;
};
