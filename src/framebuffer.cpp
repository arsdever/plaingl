#include <glad/gl.h>
#include <glm/vec2.hpp>

#include "framebuffer.hpp"

#include "logging.hpp"
#include "texture.hpp"

namespace
{
logger log() { return get_logger("framebuffer"); }
} // namespace

struct framebuffer::private_data
{
    glm::uvec2 _size { 0, 0 };
    unsigned _fbo { 0 };
    std::shared_ptr<texture> _color_texture { std::make_shared<texture>() };
    std::shared_ptr<texture> _depth_texture { std::make_shared<texture>() };
};

framebuffer::framebuffer() { _p = std::make_unique<private_data>(); }

framebuffer::~framebuffer() { destroy(); }

void framebuffer::initialize()
{
    if (_p->_fbo == 0)
    {
        glGenFramebuffers(1, &_p->_fbo);
    }

    log()->info("Framebuffer {}:\n\ttextures::color {}\n\ttextures::depth{}",
                _p->_fbo,
                _p->_color_texture->native_id(),
                _p->_depth_texture->native_id());

    bind();
    _p->_color_texture->reinit(_p->_size.x, _p->_size.y, texture::format::RGBA);
    _p->_depth_texture->reinit(
        _p->_size.x, _p->_size.y, texture::format::DEPTH);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _p->_color_texture->native_id(),
                           0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           _p->_depth_texture->native_id(),
                           0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        log()->error("Framebuffer {} is not complete!", _p->_fbo);
        unbind();
        return;
    }

    std::array<unsigned, 1> buffers { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(buffers.size(), buffers.data());
}

void framebuffer::destroy() { glDeleteFramebuffers(1, &_p->_fbo); }

std::shared_ptr<const texture> framebuffer::color_texture() const
{
    return _p->_color_texture;
}

std::shared_ptr<const texture> framebuffer::depth_texture() const
{
    return _p->_depth_texture;
}

void framebuffer::resize(glm::uvec2 size)
{
    if (_p->_size != size)
    {
        _p->_size = size;
        _p->_color_texture->reinit(size.x, size.y, texture::format::RGBA);
        _p->_depth_texture->reinit(size.x, size.y, texture::format::DEPTH);
    }
}

void framebuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, _p->_fbo); }

void framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
