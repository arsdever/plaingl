#include "renderer/material_preview_renderer.hpp"

#include "asset_manager.hpp"
#include "camera.hpp"
#include "framebuffer.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mesh.hpp"

namespace
{
logger log() { return get_logger("material_preview_renderer"); }
}; // namespace

material_preview_renderer::material_preview_renderer() = default;
material_preview_renderer::~material_preview_renderer() = default;

void material_preview_renderer::render(material& mat)
{
    auto preview_mesh = asset_manager::default_asset_manager()->get_mesh(
        "Shader Ball JL 01_mesh");
    glm::mat4 model = glm::identity<glm::mat4>();

    glm::mat4 view = glm::lookAt(
        glm::vec3 { 2, 2, 2 }, glm::vec3 { 0, .3, 0 }, glm::vec3 { 0, 1, 0 });

    glm::vec2 size { 512, 512 };
    glm::mat4 proj = glm::perspective(.6f, size.x / size.y, 0.1f, 10000.0f);

    mat.set_property_value("u_model_matrix", model);
    mat.set_property_value("u_vp_matrix", proj * view);
    mat.set_property_value("u_camera_position", glm::vec3 { 2, 2, 2 });
    mat.activate();

    struct glsl_lights_t
    {
        glm::vec3 position;
        float intensity;
        glm::vec3 direction;
        float radius;
        glm::vec3 color;
        uint32_t type;
    };
    std::vector<glsl_lights_t> glsl_lights;
    glsl_lights.resize(1);

    unsigned lights_buffer = 0;
    glGenBuffers(1, &lights_buffer);
    glsl_lights[ 0 ].position = { 2, 2, 2 };
    glsl_lights[ 0 ].direction = glm::normalize(glm::vec3 { 3, 3, 3 });
    glsl_lights[ 0 ].color = glm::vec3 { 1, 1, 1 };
    glsl_lights[ 0 ].intensity = 10.0f;
    glsl_lights[ 0 ].radius = 1.0f;
    glsl_lights[ 0 ].type = 0;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lights_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 4 * 3 * sizeof(float),
                 glsl_lights.data(),
                 GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lights_buffer);

    _fb->resize(size);
    _fb->bind();

    glViewport(0, 0, 512, 512);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    preview_mesh->render();

    _fb->unbind();
    glDeleteBuffers(1, &lights_buffer);
}

bool material_preview_renderer::initialize()
{
    if (!is_context_available())
    {
        log()->error("A OpenGL context is not available");
        return false;
    }

    _fb = std::make_unique<framebuffer>();
    _fb->resize(glm::uvec2 { 512, 512 });
    _fb->initialize();
    return true;
}

void material_preview_renderer::get_result(texture& txt) const
{
    _fb->copy_texture(&txt);
}

void material_preview_renderer::set_model_matrix(glm::mat4 model_matrix)
{
    _model_matrix = std::move(model_matrix);
}

void material_preview_renderer::set_vp_matrix(glm::mat4 vp_matrix)
{
    _vp_matrix = std::move(vp_matrix);
}

bool material_preview_renderer::is_context_available() const
{
    return glfwGetCurrentContext() != nullptr;
}
