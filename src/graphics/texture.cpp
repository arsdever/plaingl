// TODO: investigate PBO and integrate
// https://www.songho.ca/opengl/gl_pbo.html

#include <glad/gl.h>

#include "graphics/texture.hpp"

#include "common/logging.hpp"
#include "common/utils.hpp"
#include "graphics/image.hpp"

namespace
{
logger log() { return get_logger("texture"); }
} // namespace

template <>
texture::sampling_mode
gl_convert<texture::sampling_mode, int>(int gl_sampling_mode)
{
    using sampling_mode = texture::sampling_mode;
    switch (gl_sampling_mode)
    {
    case GL_NEAREST: return sampling_mode::nearest;
    case GL_LINEAR: return sampling_mode::linear;
    case GL_NEAREST_MIPMAP_NEAREST: return sampling_mode::nearest_nearest;
    case GL_NEAREST_MIPMAP_LINEAR: return sampling_mode::nearest_linear;
    case GL_LINEAR_MIPMAP_NEAREST: return sampling_mode::linear_nearest;
    case GL_LINEAR_MIPMAP_LINEAR: return sampling_mode::linear_linear;
    default: return sampling_mode::nearest;
    }
}

template <>
texture::wrapping_mode
gl_convert<texture::wrapping_mode, int>(int gl_wrapping_mode)
{
    using wrapping_mode = texture::wrapping_mode;
    switch (gl_wrapping_mode)
    {
    case GL_REPEAT: return wrapping_mode::repeat;
    case GL_MIRRORED_REPEAT: return wrapping_mode::mirrored_repeat;
    case GL_CLAMP_TO_EDGE: return wrapping_mode::clamp_to_edge;
    case GL_CLAMP_TO_BORDER: return wrapping_mode::clamp_to_border;
    default: return wrapping_mode::repeat;
    }
}

texture::texture()
{
    glGenTextures(1, &_texture_id);
    _textures.push_back(this);
    log()->debug("New texture created {}. Total number of textures {}",
                 _texture_id,
                 _textures.size());
}

texture::texture(texture&& other)
{
    _texture_id = other._texture_id;
    _width = other._width;
    _height = other._height;
    _format = other._format;
    other._texture_id = 0;
    _textures.push_back(this);
}

texture& texture::operator=(texture&& other)
{
    _texture_id = other._texture_id;
    _width = other._width;
    _height = other._height;
    _format = other._format;
    other._texture_id = 0;
    return *this;
}

texture::~texture()
{
    if (_texture_id == 0)
    {
        log()->debug("Deleting texture {}.", _texture_id);
        glDeleteTextures(1, &_texture_id);
        _textures.erase(std::find(_textures.begin(), _textures.end(), this));
        log()->debug("Total number of textures left {}", _textures.size());
    }
}

void texture::init(size_t width, size_t height, format texture_format)
{
    _width = width;
    _height = height;
    if (_width == 0 || _height == 0)
    {
        return;
    }
    _format = texture_format;
    glBindTexture(target(), _texture_id);
    if (_samples > 1)
    {
        glTexImage2DMultisample(target(),
                                _samples,
                                convert_to_gl_internal_format(texture_format),
                                _width,
                                _height,
                                GL_TRUE);
    }
    else
    {
        glTexImage2D(target(),
                     0,
                     convert_to_gl_internal_format(texture_format),
                     _width,
                     _height,
                     0,
                     convert_to_gl_format(texture_format),
                     GL_UNSIGNED_BYTE,
                     nullptr);
    }

    auto error = glGetError();
    if (error != GL_NO_ERROR)
    {
        log()->error("Error ocurred {}", error);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return;
}

void texture::set_samples(int sample_count)
{
    _samples = sample_count;
    init(_width, _height, _format);
}

int texture::get_samples() { return _samples; }

glm::uvec2 texture::get_size() const { return { _width, _height }; }

size_t texture::get_width() const { return _width; }

size_t texture::get_height() const { return _height; }

size_t texture::get_channel_count() const
{
    switch (_format)
    {
    case format::GRAYSCALE:
    case format::DEPTH: return 1;
    case format::RGB: return 3;
    case format::RGBA: return 4;
    default: return 0;
    }
}

void texture::get_data(char* data_ptr)
{
    glBindTexture(target(), _texture_id);
    glGetTexImage(target(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data_ptr);
    glBindTexture(target(), 0);
}

void texture::set_data(const char* data_ptr)
{
    set_rect_data({ 0, 0 }, { _width, _height }, data_ptr);
}

void texture::set_rect_data(glm::vec<2, size_t> pos,
                            glm::vec<2, size_t> size,
                            const char* data_ptr)
{
    glBindTexture(target(), _texture_id);
    glTexSubImage2D(target(),
                    0,
                    pos.x,
                    pos.y,
                    size.x,
                    size.y,
                    convert_to_gl_format(_format),
                    GL_UNSIGNED_BYTE,
                    data_ptr);
}

void texture::bind() const { static_bind(_texture_id, _samples > 1); }

void texture::unbind() const { static_unbind(_samples > 1); }

void texture::set_active_texture(size_t index) const
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
}

void texture::set_wrapping_mode(bool x, bool y, wrapping_mode mode)
{
    int wmode = GL_REPEAT;
    switch (mode)
    {
    case wrapping_mode::repeat: break;
    case wrapping_mode::clamp_to_border: wmode = GL_CLAMP_TO_BORDER; break;
    case wrapping_mode::clamp_to_edge: wmode = GL_CLAMP_TO_EDGE; break;
    case wrapping_mode::mirrored_repeat: wmode = GL_MIRRORED_REPEAT; break;
    }

    if (x)
        glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_S, wmode);
    if (y)
        glTextureParameteri(_texture_id, GL_TEXTURE_WRAP_T, wmode);
}

texture::wrapping_mode texture::get_wrapping_mode_x() const
{
    int mode = 0;
    glGetTextureParameteriv(_texture_id, GL_TEXTURE_WRAP_S, &mode);
    return gl_convert<texture::wrapping_mode, int>(std::move(mode));
}

texture::wrapping_mode texture::get_wrapping_mode_y() const
{
    int mode = 0;
    glGetTextureParameteriv(_texture_id, GL_TEXTURE_WRAP_T, &mode);
    return gl_convert<texture::wrapping_mode, int>(std::move(mode));
}

void texture::set_sampling_mode_min(sampling_mode mode)
{
    switch (mode)
    {
    case sampling_mode::nearest:
        glTextureParameteri(_texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    case sampling_mode::linear:
        glTextureParameteri(_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case sampling_mode::nearest_nearest:
        glTextureParameteri(
            _texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        break;
    case sampling_mode::nearest_linear:
        glTextureParameteri(
            _texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        break;
    case sampling_mode::linear_nearest:
        glTextureParameteri(
            _texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    case sampling_mode::linear_linear:
        glTextureParameteri(
            _texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
}

texture::sampling_mode texture::get_sampling_mode_min() const
{
    int mode = 0;
    glGetTextureParameteriv(_texture_id, GL_TEXTURE_MIN_FILTER, &mode);
    return gl_convert<texture::sampling_mode, int>(std::move(mode));
}

void texture::set_sampling_mode_mag(sampling_mode mode)
{
    switch (mode)
    {
    case sampling_mode::nearest:
    case sampling_mode::nearest_nearest:
    case sampling_mode::nearest_linear:
        glTextureParameteri(_texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case sampling_mode::linear:
    case sampling_mode::linear_nearest:
    case sampling_mode::linear_linear:
        glTextureParameteri(_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }
}

texture::sampling_mode texture::get_sampling_mode_mag() const
{
    int mode = 0;
    glGetTextureParameteriv(_texture_id, GL_TEXTURE_MAG_FILTER, &mode);
    return gl_convert<texture::sampling_mode, int>(std::move(mode));
}

void texture::clone(const texture* other_texture)
{
    if (get_width() != other_texture->get_width() ||
        get_height() != other_texture->get_height() ||
        _format != other_texture->_format)
    {
        init(other_texture->get_width(),
             other_texture->get_height(),
             other_texture->_format);
    }

    glCopyImageSubData(other_texture->_texture_id,
                       GL_TEXTURE_2D,
                       0,
                       0,
                       0,
                       0,
                       _texture_id,
                       GL_TEXTURE_2D,
                       0,
                       0,
                       0,
                       0,
                       get_width(),
                       get_height(),
                       1);
}

unsigned texture::native_id() const { return _texture_id; }

texture texture::from_image(image* img)
{
    auto md = img->get_metadata();
    texture result;
    format tformat = format::UNSPECIFIED;
    switch (md._color_type)
    {
    case image::color_type::GRAYSCALE:
    {
        tformat = format::GRAYSCALE;
        break;
    }
    case image::color_type::RGB:
    {
        tformat = format::RGB;
        break;
    }
    case image::color_type::RGBA:
    {
        tformat = format::RGBA;
        break;
    }
    default:
    {
        break;
    }
    }
    result.init(img->get_width(), img->get_height(), tformat);
    result.set_data(img->get_data());
    return result;
}

void texture::static_bind(size_t id, bool ms)
{
    glBindTexture(ms ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, id);
}

void texture::static_unbind(bool ms)
{
    glBindTexture(ms ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 0);
}

int texture::convert_to_gl_internal_format(format f)
{
    switch (f)
    {
    case format::DEPTH: return GL_DEPTH_COMPONENT;
    case format::GRAYSCALE: return GL_RED;
    case format::RGB: return GL_RGB;
    case format::RGBA: return GL_RGBA;
    default: return GL_NONE;
    }
}

int texture::convert_to_gl_format(format f)
{
    switch (f)
    {
    case format::DEPTH: return GL_DEPTH_COMPONENT;
    case format::GRAYSCALE: return GL_RED;
    case format::RGB: return GL_RGB;
    case format::RGBA: return GL_RGBA;
    default: return GL_NONE;
    }
}

unsigned texture::target() const
{
    return _samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

std::vector<texture*> texture::_textures;
