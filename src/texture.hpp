#pragma once

#include "color.hpp"

class texture
{
public:
    texture();
    texture(const texture& other) = delete;
    texture(texture&& other);
    texture& operator=(const texture& other) = delete;
    texture& operator=(texture&& other);
    ~texture();

    void init(size_t width, size_t height, const void* data);
    void bind(size_t index) const;

    unsigned id() const;

private:
    size_t _width;
    size_t _height;
    unsigned _texture_id;
};
