#include "asset_manager.hpp"
#include "camera.hpp"
#include "game_object.hpp"
#include "gl_window.hpp"
#include "image.hpp"
#include "logging.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "texture_viewer.hpp"

#include <GLFW\glfw3.h>

namespace
{
static inline logger log() { return get_logger("material_editor"); }
} // namespace

void on_error(int error_code, const char* description)
{
    log()->error("Error {}: {}", error_code, description);
}

int main(int argc, char** argv)
{
    glfwInit();
    glfwSetErrorCallback(on_error);

    gl_window main_window;
    main_window.resize(500, 500);
    main_window.init();

    scene s;
    camera c;
    main_window.set_camera(&c);

    auto* am = asset_manager::default_asset_manager();
    am->load_asset("sphere.fbx");
    am->load_asset("sample.png");
    am->load_asset("material.mat");
    game_object* object = new game_object();

    c.get_transform().set_position({ 0, 3, 3 });

    object->set_mesh(am->meshes()[ 0 ]);
    object->set_material(am->get_material("material"));
    image* sample_image = am->get_image("sample");

    texture* t = new texture;
    t->init(sample_image->get_width(),
            sample_image->get_height(),
            sample_image->get_data());
    object->get_material()->set_property_value("ambient_texture", t);

    s.add_object(object);

    bool should_exit = false;
    main_window.on_window_closed +=
        [ &should_exit ](auto) { should_exit = true; };
    main_window.on_keypress +=
        [ am, object, t ](gl_window* window, int key_code)
    {
        if (key_code == GLFW_KEY_SPACE)
        {
            am->update("base.shader");
            am->update("material.mat");
            object->set_material(am->get_material("material"));
            object->get_material()->set_property_value("ambient_texture", t);
        }
    };

    while (!should_exit)
    {
        main_window.update();
    }

    return 0;
}
