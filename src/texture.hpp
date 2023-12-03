#pragma once

#include "color.hpp"

class texture
{
public:
    texture(size_t width, size_t height);

    void init();

    void set_pixel(size_t x, size_t y, color c);
    color get_pixel(size_t x, size_t y);

    unsigned id() const;

private:
    size_t _width;
    size_t _height;
    unsigned _texture_id;
};
