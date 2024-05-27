#include "experimental/editor_window.hpp"

#include "asset_manager.hpp"
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

    get_events().key_press += [ this ](auto ke)
    {
        // control local camera movement
    };
    get_events().render += [ this ](auto re) { render_grid(); };
}

void editor_window::render_grid()
{
    // TODO: need to calculate from the camera
    glm::mat4 view = glm::lookAt({ 5, 5, 5 }, { 0, 0, 0 }, glm::vec3(0, 1, 0));
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    glViewport(0, 0, get_width(), get_height());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderer_3d::draw_grid(0.3f, glm::identity<glm::mat4>(), view, projection);
}

} // namespace experimental
