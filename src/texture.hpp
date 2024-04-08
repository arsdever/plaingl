#pragma once

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

    void set_samples(int sample_count);

    glm::uvec2 get_size() const;
    size_t get_width() const;
    size_t get_height() const;
    size_t get_channel_count() const;
    void get_data(char* data_ptr);
    void set_data(const char* data_ptr);
    void set_rect_data(glm::vec<2, size_t> pos,
                       glm::vec<2, size_t> size,
                       const char* data_ptr);
    void bind() const;
    void unbind() const;
    void set_active_texture(size_t index) const;

    void clone(const texture* other_texture);

    unsigned native_id() const;

    static texture from_image(image* img);

    static void static_bind(size_t id, bool ms);
    static void static_unbind(bool ms);

private:
    static int convert_to_gl_internal_format(format f);
    static int convert_to_gl_format(format f);
    unsigned target() const;

private:
    size_t _width { 0 };
    size_t _height { 0 };
    unsigned _texture_id { 0 };
    format _format { format::UNSPECIFIED };
    int _samples { 1 };

public:
    // TODO: may not be the best place for this object
    static std::vector<texture*> _textures;
};
