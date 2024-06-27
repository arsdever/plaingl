#include "graphics/image.hpp"

#include "graphics/texture.hpp"

image::image() = default;

void image::init(metadata md) { _metadata = md; }

void image::init(size_t width, size_t height) { }

void image::set_data(std::vector<char> data) { _data_buffer = std::move(data); }

void image::set_data(const char* data)
{
    size_t raw_data_size = _metadata._bytes_per_row * _metadata._height;
    set_data(data, raw_data_size);
}

void image::set_data(const char* data, size_t size)
{
    _data_buffer = std::vector<char>(data, data + size);
}

template <>
const char* image::get_data<char>() const
{
    return _data_buffer.data();
}

template <>
const unsigned char* image::get_data<unsigned char>() const
{
    return reinterpret_cast<const unsigned char*>(_data_buffer.data());
}

template <>
char* image::raw_data<char>()
{
    return _data_buffer.data();
}

template <>
unsigned char* image::raw_data<unsigned char>()
{
    return reinterpret_cast<unsigned char*>(_data_buffer.data());
}

const image::metadata& image::get_metadata() const { return _metadata; }

size_t image::get_width() const { return _metadata._width; }

size_t image::get_height() const { return _metadata._height; }

image* image::from_texture(texture* txt)
{
    image* result = new image;
    image::metadata md;
    md._width = txt->get_width();
    md._height = txt->get_height();
    md._bits_per_pixel = 32;
    md._bytes_per_row = md._width * 4;
    md._channel_count = 4;
    md._color_type = image::color_type::RGBA;
    md._file_format = image::file_format::PNG;
    result->init(md);
    result->_data_buffer.resize(md._height * md._bytes_per_row);
    txt->get_data(result->raw_data());
    return result;
}
