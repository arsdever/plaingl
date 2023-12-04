#include <glad/gl.h>

#include <string>
#include "texture.hpp"

texture::texture(size_t width, size_t height)
    : _width(width)
    , _height(height)
{
}

void texture::init()
{
    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB32UI,
                 _width,
                 _height,
                 0,
                 GL_RGB_INTEGER,
                 GL_UNSIGNED_INT,
                 nullptr);
    auto error = glGetError();
    if (error != GL_NO_ERROR)
    {
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return;
}

void texture::set_pixel(size_t x, size_t y, color c)
{
    if (x > _width || y > _height)
    {
        return;
    }
}

color texture::get_pixel(size_t x, size_t y)
{
    // TODO: implement
    if (x > _width || y > _height)
    {
        return {};
    }

    return {};
}

unsigned texture::id() const { return _texture_id; }
