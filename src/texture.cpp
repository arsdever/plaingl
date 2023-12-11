// TODO: investigate PBO and integrate
// https://www.songho.ca/opengl/gl_pbo.html

#include <string>

#include <glad/gl.h>

#include "texture.hpp"

texture::texture() = default;

texture::texture(texture&& other)
{
    _texture_id = other._texture_id;
    other._texture_id = 0;
}

texture& texture::operator=(texture&& other)
{
    _texture_id = other._texture_id;
    other._texture_id = 0;
    return *this;
}

texture::~texture() { glDeleteTextures(1, &_texture_id); }

void texture::init(size_t width, size_t height, const void* image_data)
{
    _width = width;
    _height = height;
    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 _width,
                 _height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image_data);
    auto error = glGetError();
    if (error != GL_NO_ERROR)
    {
        return;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return;
}

void texture::bind(size_t index) const
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
}

unsigned texture::id() const { return _texture_id; }
