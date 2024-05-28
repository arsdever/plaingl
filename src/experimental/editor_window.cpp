#include "experimental/editor_window.hpp"

#include "asset_manager.hpp"
#include "experimental/input_system.hpp"
#include "renderer_3d.hpp"

namespace experimental
{

editor_window::editor_window()
{
    on_user_initialize += [ this ](auto) { initialize(); };
}

editor_window::~editor_window() { }

void editor_window::initialize()
{
    asset_manager::default_asset_manager()->load_asset(
        "resources/standard/grid.shader");
    asset_manager::default_asset_manager()->load_asset(
        "resources/standard/line.shader");

    _camera_position = { 5, 5, 5 };

    input_system::on_keypress += [ this ](int keycode)
    {
        // control local camera movement
        switch (keycode)
        {
        case GLFW_KEY_D:
        {
            _camera_position.x += .1;
            break;
        }
        case GLFW_KEY_A:
        {
            _camera_position.x -= .1;
            break;
        }
        case GLFW_KEY_SPACE:
        {
            _camera_position.y += .1;
            break;
        }
        case GLFW_KEY_LEFT_CONTROL:
        {
            _camera_position.y -= .1;
            break;
        }
        case GLFW_KEY_W:
        {
            _camera_position.z += .1;
            break;
        }
        case GLFW_KEY_S:
        {
            _camera_position.z -= .1;
            break;
        }
        }
    };
    get_events().render += [ this ](auto re) { render_grid(); };
}

void editor_window::render_grid()
{
    // TODO: need to calculate from the camera
    glm::mat4 view =
        glm::lookAt(_camera_position, { 0, 0, 0 }, glm::vec3(0, 1, 0));
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    glViewport(0, 0, get_width(), get_height());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer_3d::draw_grid(0.3f, glm::identity<glm::mat4>(), view, projection);

    unsigned axis_viewport_size = 80;
    glViewport(get_width() - axis_viewport_size - 20,
               get_height() - axis_viewport_size - 20,
               axis_viewport_size,
               axis_viewport_size);
    view = glm::lookAt(glm::normalize(_camera_position) * 3.0f,
                       { 0, 0, 0 },
                       glm::vec3(0, 1, 0));
    renderer_3d::draw_ray(glm::vec3 { 0, 0, 0 },
                          glm::vec3 { 1, 0, 0 },
                          1.0f,
                          { 1, 0, 0, 1 },
                          view,
                          projection);
    renderer_3d::draw_ray(glm::vec3 { 0, 0, 0 },
                          glm::vec3 { 0, 1, 0 },
                          1.0f,
                          { 0, 1, 0, 1 },
                          view,
                          projection);
    renderer_3d::draw_ray(glm::vec3 { 0, 0, 0 },
                          glm::vec3 { 0, 0, 1 },
                          1.0f,
                          { 0, 0, 1, 1 },
                          view,
                          projection);
}

} // namespace experimental
