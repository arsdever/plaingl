/* clang-format off */
#include <GLFW/glfw3.h>
/* clang-format on */

#include "core/viewport.hpp"

#include "asset_management/asset_manager.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "core/window.hpp"
#include "graphics/graphics.hpp"
#include "graphics/material.hpp"
#include "graphics/texture.hpp"
#include "project/components/camera.hpp"
#include "project/components/light.hpp"
#include "project/components/mesh_filter.hpp"
#include "project/components/mesh_renderer.hpp"
#include "project/components/transform.hpp"
#include "project/game_object.hpp"
#include "project/project_manager.hpp"
#include "project/scene.hpp"

namespace
{
logger log() { return get_logger("window example"); }
} // namespace

void init_scene();

int main(int argc, char** argv)
{
    project_manager::initialize();
    glfwInit();
    std::vector<std::shared_ptr<core::window>> windows;
    auto exp_window = std::make_shared<core::window>();

    std::shared_ptr<game_object> main_camera_object = nullptr;
    std::shared_ptr<scene> current_scene = nullptr;

    exp_window->on_user_initialize += [ &current_scene, &main_camera_object ](
                                          std::shared_ptr<core::window> wnd)

    {
        graphics::initialize();
        assets::asset_manager::initialize(
            (common::filesystem::path::current_dir() / "resources")
                .full_path());
        main_camera_object = game_object::create();
        current_scene = scene::create();
        main_camera_object->add<components::camera>().set_active();
        init_scene();
        auto vp = std::make_shared<core::viewport>();
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
            graphics::clear(glm::dvec4 { 0, 0, 0, 1 });
            int i = 0;
            for (auto vp : re.get_sender()->get_viewports())
            {
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
    auto basic_mat = assets::asset_manager::get<graphics::material>("standard");
    auto txt = assets::asset_manager::get<texture>("diffuse");
    auto norm = assets::asset_manager::get<texture>("brick");
    basic_mat->set_property_value("albedo_texture_strength", 0.0f);
    basic_mat->set_property_value("albedo_color", 1.0f, 0.8f, 0.2f);
    basic_mat->set_property_value("normal_texture_strength", 0.0f);

    components::camera::get_active()->get_transform()->set_position(
        { 0, 0, 3 });
    components::camera::get_active()->get_transform()->set_rotation(
        glm::quatLookAt(glm::vec3 { 0.0f, 0.0f, 1.0f },
                        glm::vec3 {
                            0.0f,
                            1.0f,
                            0.0f,
                        }));
    components::camera::get_active()->set_orthogonal(false);
    components::camera::get_active()->set_background_color(
        glm::dvec4(0.3, 0.6, 0.7, 1.0));

    auto obj = game_object::create();
    auto m = assets::asset_manager::get<mesh>("susane_head");
    obj->add<components::mesh_filter>().set_mesh(m.get());
    obj->add<components::mesh_renderer>().set_material(basic_mat);
    obj->set_name("susane");
    scene::get_active_scene()->add_root_object(obj);

    auto lo = game_object::create();
    auto& l = lo->add<components::light>();
    l.set_color(glm::dvec4(1.0));
    l.set_intensity(5.0);
}
