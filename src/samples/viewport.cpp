/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <any>
#include <functional>
#include <memory>
#include <vector>

#include "experimental/viewport.hpp"

#include "asset_manager.hpp"
#include "experimental/window.hpp"
#include "gl_error_handler.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "project/components/camera.hpp"
#include "project/components/light.hpp"
#include "project/components/mesh_filter.hpp"
#include "project/components/mesh_renderer.hpp"
#include "project/components/transform.hpp"
#include "project/game_object.hpp"
#include "project/scene.hpp"
#include "texture.hpp"

namespace
{
logger log() { return get_logger("window example"); }
} // namespace

void init_scene();

int main(int argc, char** argv)
{
    glfwInit();
    std::vector<std::shared_ptr<experimental::window>> windows;
    auto exp_window = std::make_shared<experimental::window>();

    std::shared_ptr<game_object> main_camera_object = nullptr;
    std::shared_ptr<scene> current_scene = nullptr;

    exp_window->on_user_initialize +=
        [ &current_scene,
          &main_camera_object ](std::shared_ptr<experimental::window> wnd)
    {
        // configure gl debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_error_handler, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        main_camera_object = game_object::create();
        current_scene = scene::create();
        main_camera_object->add<components::camera>().set_active();
        init_scene();
        auto vp = std::make_shared<experimental::viewport>();
        vp->set_size(wnd->get_size());
        wnd->add_viewport(vp);
    };

    exp_window->set_title("Hello experimental window");
    exp_window->init();

    exp_window->set_can_grab(true);

    exp_window->get_events()->close += [ &windows ](auto ce)
    { std::erase(windows, ce.get_sender()->shared_from_this()); };

    exp_window->get_events()->resize += [](auto re)
    {
        auto wnd = re.get_sender();
        if (auto vp = wnd->get_viewports()[ 0 ])
        {
            vp->set_size(wnd->get_size());
        }
    };

    int frame_counter = 0;
    exp_window->get_events()->render += [ &frame_counter ](auto re)
    {
        if (frame_counter % 30 == 0)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            int i = 0;
            for (auto vp : re.get_sender()->get_viewports())
            {
                vp->render();
                vp->take_screenshot(std::format(
                    "screenshots/screenshot_{}_{}.png", i, frame_counter));
                ++i;
            }
        }
        ++frame_counter;
    };

    windows.push_back(exp_window);

    while (!windows.empty())
    {
        for (int i = 0; i < windows.size(); ++i)
        {
            auto wnd = windows[ i ];
            wnd->update();
        }
    }

    return 0;
}

void init_scene()
{
    auto* am = asset_manager::default_asset_manager();
    am->load_asset("susane_head.fbx");
    am->load_asset("basic.mat");
    am->load_asset("brick.png");
    am->load_asset("diffuse.png");

    material* basic_mat = am->get_material("basic");
    auto txt = new texture();
    image* img = am->get_image("diffuse");
    *txt = std::move(texture::from_image(img));
    auto norm_txt = new texture();
    img = am->get_image("brick");
    *norm_txt = std::move(texture::from_image(img));
    basic_mat->set_property_value("albedo_texture_strength", 0.0f);
    basic_mat->set_property_value("albedo_color", 1.0f, 0.8f, 0.2f);
    basic_mat->set_property_value("normal_texture_strength", 0.0f);

    components::camera::get_active()->get_transform().set_position({ 0, 0, 3 });
    components::camera::get_active()->get_transform().set_rotation(
        glm::quatLookAt(glm::vec3 { 0.0f, 0.0f, 1.0f },
                        glm::vec3 {
                            0.0f,
                            1.0f,
                            0.0f,
                        }));
    components::camera::get_active()->set_orthogonal(false);
    components::camera::get_active()->set_background(
        glm::dvec4(0.3, 0.6, 0.7, 1.0));

    auto obj = game_object::create();
    obj->add<components::mesh_filter>().set_mesh(am->get_mesh("susane_head"));
    obj->add<components::mesh_renderer>().set_material(basic_mat);
    obj->set_name("susane");
    scene::get_active_scene()->add_root_object(obj);

    auto lo = game_object::create();
    auto& l = lo->add<components::light>();
    l.set_color(glm::dvec4(1.0));
    l.set_intensity(5.0);
}
