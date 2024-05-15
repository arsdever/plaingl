#pragma once

#include "renderer/renderer.hpp"

class material;

class material_preview_renderer : public renderer
{
public:
    material_preview_renderer();
    ~material_preview_renderer();

    void render(material& mat);

    bool initialize();
    void get_result(texture& txt) const;
    void set_model_matrix(glm::mat4 model_matrix);
    void set_vp_matrix(glm::mat4 vp_matrix);

private:
    bool is_context_available() const;

private:
    std::unique_ptr<framebuffer> _fb;
    glm::mat4 _model_matrix;
    glm::mat4 _vp_matrix;
};
