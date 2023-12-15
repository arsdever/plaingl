#pragma once

#include <string>

#include <glm/vec2.hpp>

class camera;

class viewport
{
public:
    void init();
    void set_active();

    size_t get_width() const;
    size_t get_height() const;
    glm::uvec2 get_resolution() const;
    void set_size(size_t width, size_t height);
    void set_size(glm::uvec2 size);

    glm::uvec2 get_position() const;
    void set_position(size_t x, size_t y);
    void set_position(glm::uvec2 size);

    camera* get_camera() const;
    void set_camera(camera* viewport_camera);

    void set_name(std::string_view name);
    std::string_view get_name();

    void update();

private:
    void draw() const;

private:
    glm::uvec2 _resolution {};
    glm::uvec2 _position {};
    camera* _viewport_camera;
    std::string _name;
};
