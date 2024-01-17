#pragma once

#include <glm/vec2.hpp>
#include <vector>

class image;

class texture
{
public:
    enum class format : char
    {
        UNSPECIFIED,
        GRAYSCALE,
        DEPTH,
        RGB,
        RGBA,
    };

public:
    texture();
    texture(const texture& other) = delete;
    texture(texture&& other);
    texture& operator=(const texture& other) = delete;
    texture& operator=(texture&& other);
    ~texture();

    void init(size_t width, size_t height, format texture_format = format::RGB);
    void reinit(size_t width, size_t height, format texture_format = format::RGB);

    glm::uvec2 get_size() const;
    size_t get_width() const;
    size_t get_height() const;
    size_t get_channel_count() const;
    void get_data(char* data_ptr);
    void set_data(const char* data_ptr);
    void set_rect_data(glm::vec<2, size_t> pos,
                       glm::vec<2, size_t> size,
                       const char* data_ptr);
    void bind(size_t index) const;

    unsigned native_id() const;

    static texture from_image(image* img);

private:
    static int convert_to_gl_internal_format(format f);
    static int convert_to_gl_format(format f);

private:
    size_t _width;
    size_t _height;
    unsigned _texture_id;
    format _format;

public:
    // TODO: may not be the best place for this object
    static std::vector<texture*> _textures;
};
