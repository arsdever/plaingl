#pragma once

enum class graphics_buffer_type
{
    vertex,
    index,
};

class graphics_buffer
{
public:
    graphics_buffer(graphics_buffer_type type);
    graphics_buffer(graphics_buffer&& o);
    graphics_buffer(const graphics_buffer& o) = delete;
    graphics_buffer& operator=(graphics_buffer&& o);
    graphics_buffer& operator=(const graphics_buffer& o) = delete;
    ~graphics_buffer();

    void set_data(void* data_buffer);
    void get_data(void* data_buffer) const;

    void set_element_count(int element_count);
    int get_element_count() const;
    void set_element_stride(int element_stride);
    int get_element_stride() const;
    unsigned get_handle() const;

    void release();

private:
    graphics_buffer_type _type { graphics_buffer_type::vertex };
    unsigned _handle { 0 };
    int _element_count { 0 };
    int _element_stride { 0 };
};
