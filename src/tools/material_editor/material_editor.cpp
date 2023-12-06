#include "asset_manager.hpp"
#include "game_object.hpp"
#include "gl_window.hpp"
#include "logging.hpp"
#include "scene.hpp"

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

    asset_manager am;
    scene s;

    am.load_asset("sphere.fbx");
    am.load_asset("simple.shader");
    game_object* object = new game_object();

    object->set_mesh(am.meshes()[ 0 ]);

    s.add_object(object);

    bool should_exit = false;
    main_window.on_window_closed +=
        [ &should_exit ](auto) { should_exit = true; };

    while (!should_exit)
    {
        main_window.update();
    }
    return 0;
}
