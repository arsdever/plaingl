#include "image.hpp"

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

const image::metadata& image::get_metadata() const { return _metadata; }

size_t image::get_width() const { return _metadata._width; }

size_t image::get_height() const { return _metadata._height; }
