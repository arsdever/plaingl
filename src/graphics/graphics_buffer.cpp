#include <glad/gl.h>

#include "graphics/graphics_buffer.hpp"

graphics_buffer::graphics_buffer(type type)
    : _type(type)
{
    glCreateBuffers(1, &_handle);
    int gl_buffer_type = GL_ARRAY_BUFFER;
    switch (type)
    {
    case type::vertex: break;
    case type::index:
    {
        gl_buffer_type = GL_ELEMENT_ARRAY_BUFFER;
        break;
    }
    case type::shader_storage:
    {
        gl_buffer_type = GL_SHADER_STORAGE_BUFFER;
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
    int usage = GL_STATIC_DRAW;
    switch (_usage_type)
    {
    case usage_type::stream_draw:
    {
        usage = GL_STREAM_DRAW;
        break;
    }
    case usage_type::stream_read:
    {
        usage = GL_STREAM_READ;
        break;
    }
    case usage_type::stream_copy:
    {
        usage = GL_STREAM_COPY;
        break;
    }
    case usage_type::static_draw:
    {
        usage = GL_STATIC_DRAW;
        break;
    }
    case usage_type::static_read:
    {
        usage = GL_STATIC_READ;
        break;
    }
    case usage_type::static_copy:
    {
        usage = GL_STATIC_COPY;
        break;
    }
    case usage_type::dynamic_draw:
    {
        usage = GL_DYNAMIC_DRAW;
        break;
    }
    case usage_type::dynamic_read:
    {
        usage = GL_DYNAMIC_READ;
        break;
    }
    case usage_type::dynamic_copy:
    {
        usage = GL_DYNAMIC_COPY;
        break;
    }
    }
    glNamedBufferData(
        _handle, _element_count * _element_stride, data_buffer, usage);
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

void graphics_buffer::set_usage_type(usage_type usage_type)
{
    _usage_type = usage_type;
}

graphics_buffer::usage_type graphics_buffer::get_usage_type() const
{
    return _usage_type;
}

unsigned graphics_buffer::get_handle() const { return _handle; }

void graphics_buffer::release()
{
    glDeleteBuffers(1, &_handle);
    _handle = 0;
}
