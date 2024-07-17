#pragma once

#include "experimental/window.hpp"

class graphics_buffer;
class mesh;

class mesh_viewer : public experimental::window
{
public:
    mesh_viewer();
    ~mesh_viewer();

    void set_mesh(mesh* m);
    void set_mode(unsigned mode);

private:
    void initialize();
    void render();

private:
    mesh* _mesh { nullptr };
    std::unique_ptr<graphics_buffer> _light_buffer { nullptr };
    glm::vec2 _rotation { 0 };
    glm::vec2 _rotation_start_point { 0 };
    double _zoom { 1.0 };
    bool _draw_wireframe { false };
    unsigned _mode { 0 };
};
