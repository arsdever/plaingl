#include "tools/mesh_viewer/mesh_viewer.hpp"

#include "core/asset_manager.hpp"
#include "experimental/window.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics_buffer.hpp"
#include "project/game_object.hpp"
#include "renderer/renderer_3d.hpp"

mesh_viewer::mesh_viewer()
{
    resize(600, 600);
    on_user_initialize +=
        [ this ](std::shared_ptr<experimental::window>) { initialize(); };
}

mesh_viewer::~mesh_viewer() = default;

void mesh_viewer::set_mesh(mesh* m) { _mesh = m; }

void mesh_viewer::set_mode(unsigned m) { _mode = m; }

void mesh_viewer::initialize()
{
    struct light_t
    {
        glm::vec3 position;
        float intensity;
        glm::vec3 direction;
        float radius;
        glm::vec3 color;
        uint32_t type;
    } l[ 2 ];

    l[ 0 ].position = glm::vec3(3, 3, 3);
    l[ 0 ].color = glm::vec3(1, 1, 1);
    l[ 0 ].intensity = 10;

    l[ 1 ].position = glm::vec3(-3, -3, 3);
    l[ 1 ].color = glm::vec3(1, 1, 1);
    l[ 1 ].intensity = 15;

    _light_buffer = std::make_unique<graphics_buffer>(
        graphics_buffer::type::shader_storage);
    _light_buffer->set_element_stride(12 * sizeof(float));
    _light_buffer->set_element_count(2);
    _light_buffer->set_data(&l);

    get_events()->render += [ this ](auto e) { render(); };
    get_events()->key_press += [ this ](auto e)
    {
        if (e.get_key() == GLFW_KEY_W)
            _draw_wireframe = !_draw_wireframe;

        if (e.get_key() >= '0' && e.get_key() <= '5')
        {
            set_mode(e.get_key() - '0');
        }
    };
    get_events()->mouse_press +=
        [ this ](auto e) { _rotation_start_point = e.get_local_position(); };
    get_events()->mouse_move += [ this ](auto e)
    {
        if (e.get_buttons() & (1 << GLFW_MOUSE_BUTTON_1))
        {
            _rotation.x += e.get_local_position().y - _rotation_start_point.y;
            _rotation.y += e.get_local_position().x - _rotation_start_point.x;
            _rotation_start_point = e.get_local_position();
        }
    };
    get_events()->resize +=
        [ this ](auto e) { resize(e.get_new_size().x, e.get_new_size().y); };
    glEnable(GL_DEPTH_TEST);
}

void mesh_viewer::render()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _light_buffer->get_handle());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _light_buffer->get_handle());

    auto mat =
        asset_manager::default_asset_manager()->get_material("mesh_viewer");
    auto camera_position = glm::vec3(0, 0, 8);

    glm::mat4 camera_matrix =
        glm::perspective(glm::radians(30.0f),
                         static_cast<float>(get_width()) /
                             static_cast<float>(get_height()),
                         0.1f,
                         100.0f) *
        glm::lookAt(camera_position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    mat->set_property_value("u_vp_matrix", camera_matrix);
    mat->set_property_value("u_model_matrix", glm::identity<glm::mat4>());
    mat->set_property_value("u_camera_position", camera_position);
    mat->set_property_value("u_mode", _mode);

    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::rotate(model, _rotation.y * 0.01f, glm::vec3(0, 1, 0));
    model = glm::rotate(model, _rotation.x * 0.01f, glm::vec3(1, 0, 0));

    mat->set_property_value("u_model_matrix", model);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, get_width(), get_height());
    glClearColor(0.0f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_mesh == nullptr)
        return;

    renderer_3d().draw_mesh(_mesh, mat);
    if (_draw_wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mat->set_property_value("u_wireframe", 1.0f);
        renderer_3d().draw_mesh(_mesh, mat);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mat->set_property_value("u_wireframe", 0.0f);
    }
}
