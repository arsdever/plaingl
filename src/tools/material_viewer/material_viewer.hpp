#pragma once

#include "graphics/graphics_fwd.hpp"

#include "core/window.hpp"

class material_viewer : public core::window
{
public:
    material_viewer();
    ~material_viewer();

    void set_mesh_presets(std::vector<std::shared_ptr<graphics::mesh>> m);
    void set_material(std::shared_ptr<graphics::material> m);

private:
    void initialize();
    void render();

private:
    std::shared_ptr<graphics::material> _material { nullptr };
    std::vector<std::shared_ptr<graphics::mesh>> _mesh_presets;
    std::shared_ptr<graphics::mesh> _mesh;
    std::unique_ptr<graphics_buffer> _light_buffer { nullptr };
    glm::vec2 _rotation { 0 };
    glm::vec2 _rotation_start_point { 0 };
    double _zoom { 1.0 };
    bool _draw_wireframe { false };
};
