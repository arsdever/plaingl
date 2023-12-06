#include "image.hpp"

image::image() = default;

void image::init(size_t width, size_t height)
{
    _width = width;
    _height = height;
}

void image::set_data(char* data) { _data = data; }

const char* image::get_data() { return _data; }

size_t image::width() const { return _width; }

size_t image::height() const { return _height; }
