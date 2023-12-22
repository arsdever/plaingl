#pragma once

#include <vector>

#include <glm/vec2.hpp>

class image
{
public:
    enum class file_format : char
    {
        UNSPECIFIED,
        PNG,
        JPG,
    };

    enum class color_type : char
    {
        UNSPECIFIED,
        GRAYSCALE,
        RGB,
        PALETTE,
        GRAYSCALE_ALPHA,
        RGBA,
        YCbCr,
        CMYK,
        YCCK,
    };

    struct metadata
    {
        size_t _width;
        size_t _height;
        size_t _bits_per_pixel;
        size_t _bytes_per_row;
        unsigned char _channel_count;
        color_type _color_type;
        file_format _file_format;
    };

public:
    image();

    void init(size_t width, size_t height);
    void init(metadata md);
    void set_data(std::vector<char> data);
    void set_data(const char* data);
    void set_data(const char* data, size_t size);
    template <typename T = char>
    const T* get_data() const;
    const metadata& get_metadata() const;

    size_t get_width() const;
    size_t get_height() const;
    glm::vec<2, size_t> get_size() const;

private:
    metadata _metadata;
    std::vector<char> _data_buffer;
};
