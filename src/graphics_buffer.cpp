#include <stdexcept>

#include "graphics_buffer.hpp"

#include "glad/gl.h"

graphics_buffer::graphics_buffer(graphics_buffer_type type)
    : _type(type)
{
    glCreateBuffers(1, &_handle);
    int gl_buffer_type = GL_ARRAY_BUFFER;
    switch (type)
    {
    case graphics_buffer_type::vertex: break;
    case graphics_buffer_type::index:
    {
        gl_buffer_type = GL_ELEMENT_ARRAY_BUFFER;
        break;
    }
    }
}

graphics_buffer::graphics_buffer(graphics_buffer&& o)
    : _type(o._type)
    , _handle(o._handle)
    , _element_count(o._element_count)
    , _element_stride(o._element_stride)
{
    o._handle = 0;
}

graphics_buffer& graphics_buffer::operator=(graphics_buffer&& o)
{
    _type = o._type;
    _handle = o._handle;
    _element_count = o._element_count;
    _element_stride = o._element_stride;
    o._handle = 0;
    return *this;
}

graphics_buffer::~graphics_buffer() { release(); }

void graphics_buffer::set_data(void* data_buffer)
{
    glNamedBufferData(
        _handle, _element_count * _element_stride, data_buffer, GL_STATIC_DRAW);
}

void graphics_buffer::get_data(void* data_buffer) const
{
    throw std::runtime_error("Not implemented");
}

void graphics_buffer::set_element_count(int element_count)
{
    _element_count = element_count;
}

int graphics_buffer::get_element_count() const { return _element_count; }

void graphics_buffer::set_element_stride(int element_stride)
{
    _element_stride = element_stride;
}

int graphics_buffer::get_element_stride() const { return _element_stride; }

unsigned graphics_buffer::get_handle() const { return _handle; }

void graphics_buffer::release() { glDeleteBuffers(1, &_handle); }
