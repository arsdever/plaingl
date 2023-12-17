/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <prof/profiler.hpp>

#include "asset_manager.hpp"
#include "camera.hpp"
#include "components/camera_component.hpp"
#include "components/fps_show_component.hpp"
#include "components/jumpy_component.hpp"
#include "components/mesh_component.hpp"
#include "components/mesh_renderer_component.hpp"
#include "components/ray_visualize_component.hpp"
#include "components/text_component.hpp"
#include "components/text_renderer_component.hpp"
#include "font.hpp"
#include "game_clock.hpp"
#include "game_object.hpp"
#include "image.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mouse_events_refiner.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "texture_viewer.hpp"
#include "thread.hpp"
#include "viewport.hpp"
#include "window.hpp"

namespace
{
static logger log() { return get_logger("main"); }
unsigned last_fps;
font ttf;
scene s;
std::unordered_set<int> pressed_keys;
camera* main_camera;
game_object* main_camera_object;
mouse_events_refiner mouse_events;
game_object* _fps_text_object;
texture* txt;
texture* norm_txt;
ray_visualize_component* cast_ray;

std::array<camera*, 3> _view_cameras { nullptr };
} // namespace

void process_console();

void initScene();

void on_error(int error_code, const char* description)
{
    log()->error("Error {}: {}", error_code, description);
}

static std::atomic_int counter = 0;

int main(int argc, char** argv)
{
    configure_levels(argc, argv);
    game_clock* clock = game_clock::init();
    glfwInit();
    glfwSetErrorCallback(on_error);
    adjust_timeout_accuracy_guard guard;

    auto* am = asset_manager::default_asset_manager();
    am->load_asset("sample.png");
    am->load_asset("sample_jpg.jpg");
    // texture_viewer::show_preview(am->get_image("sample_jpg"));
    main_camera = new camera;

    std::vector<window*> windows;

    windows.push_back(new window);
    windows.back()->init();
    windows.back()->resize(400, 400);
    windows.back()->set_active();
    windows.back()->on_window_closed += [ &windows ](window* window)
    { windows.erase(std::find(windows.begin(), windows.end(), window)); };
    std::shared_ptr<viewport> vp = std::make_shared<viewport>();
    vp->init();
    windows.back()->add_viewport(vp);
    windows.back()->get_main_viewport()->set_camera(main_camera);
    windows.back()->set_mouse_events_refiner(&mouse_events);

    mouse_events_refiner* me = new mouse_events_refiner;
    auto* wnd = new window;
    windows.push_back(wnd);
    wnd->resize(200, 600);
    wnd->init();
    wnd->set_mouse_events_refiner(me);
    struct layout_vert3 : window::layout
    {
        void calculate_layout(window* wnd) override
        {
            auto vps = wnd->get_viewports();
            for (int i = 0; i < std::min<size_t>(vps.size(), 3); ++i)
            {
                auto& vp = vps[ i ];
                vp->set_position(
                    0, (_view_cameras.size() - i - 1) * wnd->get_height() / 3);
                vp->set_size(wnd->get_width(), wnd->get_height() / 3);
            }
        }
    };

    wnd->set_layout<layout_vert3>();
    wnd->on_window_closed += [ &windows ](window* wnd)
    { windows.erase(std::find(windows.begin(), windows.end(), wnd)); };

    for (int i = 0; i < _view_cameras.size(); ++i)
    {
        auto& cam = _view_cameras[ i ];
        cam = new camera;
        std::shared_ptr<viewport> vp = std::make_shared<viewport>();
        vp->init();
        vp->set_camera(cam);
        vp->set_visible(true);
        cam->set_ortho(true);
        wnd->add_viewport(vp);
        me->scroll += [ cam, vp ](auto params)
        {
            if (params._viewport != vp)
            {
                return;
            }

            cam->get_transform().set_position(
                cam->get_transform().get_position() *
                std::pow<float>(1.2, params._delta.y));
        };
    }

    game_object* selected_object = nullptr;

    mouse_events.click +=
        [ &selected_object ](mouse_events_refiner::mouse_event_params params)
    {
        auto* object = params._window->find_game_object_at_position(
            params._position.x, params._position.y);
        log()->info("Main window clicked: Object selected {}",
                    reinterpret_cast<unsigned long long>(object));
        if (selected_object != nullptr)
        {
            selected_object->set_selected(false);
            selected_object = nullptr;
        }
        if (object != nullptr)
        {
            object->set_selected(true);
            selected_object = object;
        }
    };

    mouse_events.drag_drop_start +=
        [](mouse_events_refiner::mouse_event_params params)
    {
        glm::vec2 from = params._old_position;
        glm::vec2 to = params._position;
        from.y = params._window->get_height() - from.y;
        to.y = params._window->get_height() - to.y;
        log()->trace("dragging started from position ({}, {})", from.x, from.y);
    };
    mouse_events.drag_drop_move +=
        [](mouse_events_refiner::mouse_event_params params)
    {
        glm::vec2 to = params._position;
        to.y = params._window->get_height() - to.y;
        log()->trace("dragging to position ({}, {})", to.x, to.y);
    };
    mouse_events.drag_drop_end +=
        [](mouse_events_refiner::mouse_event_params params) {};

    mouse_events.move += [](auto params)
    {
        // draw ray casted from camera
        auto pos = params._window->get_main_viewport()
                       ->get_camera()
                       ->get_transform()
                       .get_position();
        auto rot = params._window->get_main_viewport()
                       ->get_camera()
                       ->get_transform()
                       .get_rotation();
        glm::vec3 point = glm::unProject(
            glm::vec3 { params._position.x,
                        params._window->get_height() - params._position.y,
                        0 },
            params._window->get_main_viewport()->get_camera()->view_matrix(),
            params._window->get_main_viewport()
                ->get_camera()
                ->projection_matrix(),
            glm::vec4 { 0,
                        0,
                        params._window->get_width(),
                        params._window->get_height() });
        cast_ray->set_ray(pos, glm::normalize(point - pos));
    };

    initScene();

    // TODO: may not be the best place for object initialization
    // Probably should be done in some sort of scene loading procedure
    if (scene::get_active_scene())
    {
        for (auto* obj : scene::get_active_scene()->objects())
        {
            obj->init();
        }
    }

    // start a physics thread
    // TODO: these should move into physics engine class
    std::atomic_bool program_exits = false;
    // TODO: make this variable
    static constexpr std::atomic<double> physics_fps = 500.0;
    std::thread thd { [ &program_exits, clock ]
    {
        double physics_frame_time_hint = 1.0 / physics_fps;
        while (!program_exits)
        {
            auto _physics_frame_start = std::chrono::steady_clock::now();
            // do stuff

            clock->physics_frame();
            std::this_thread::sleep_until(
                _physics_frame_start +
                std::chrono::duration<double>(physics_frame_time_hint));
        }
    } };
    set_thread_name(thd, "physics_thread");
    set_thread_priority(thd, 15);

    while (!windows.empty())
    {
        double timed_fraction =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                std::chrono::steady_clock::now().time_since_epoch())
                .count() /
            3.0f;
        main_camera_object->get_transform().set_position(
            { sin(timed_fraction) * 3.0f, cos(timed_fraction) * 3.0f, 5.0f });
        main_camera_object->get_transform().set_rotation(glm::quatLookAt(
            glm::normalize(main_camera_object->get_transform().get_position()),
            glm::vec3(0, 1, 0)));

        for (int i = 0; i < windows.size(); ++i)
        {
            auto p = prof::profile_frame(__FUNCTION__);
            auto window = windows[ i ];
            window->set_active();
            window->update();
        }
        clock->frame();
    }

    std::stringstream ss;
    ss << std::this_thread::get_id();
    prof::apply_for_data(ss.str(),
                         [](const prof::data_sample& data) -> bool
    {
        log()->info("Profiling frame:\n\tfunction name: {}\n\tdepth: "
                    "{}\n\tduration: {}",
                    data.name(),
                    data.depth(),
                    std::chrono::duration_cast<std::chrono::duration<double>>(
                        data.end() - data.start())
                        .count());
        return true;
    });
    std::cout << std::flush;
    program_exits = true;
    glfwTerminate();
    thd.join();
    return 0;
}

void initScene()
{
    shader_program* prog = new shader_program;
    prog->init();
    prog->add_shader("shader.vert");
    prog->add_shader("shader.frag");
    prog->link();

    // for (const auto& property : basic_mat->properties())
    // {
    //     log()->info("Material properties:\n\tname: {}\n\tindex: {}\n\tsize: "
    //                 "{}\n\ttype: {}",
    //                 property._name,
    //                 property._index,
    //                 property._size,
    //                 property._type);
    // }

    auto* am = asset_manager::default_asset_manager();
    am->load_asset("cube.fbx");
    am->load_asset("sphere.fbx");
    am->load_asset("susane_head.fbx");
    am->load_asset("text.mat");
    am->load_asset("basic.mat");
    am->load_asset("sample.png");
    am->load_asset("brick.png");
    am->load_asset("diffuse.png");

    material* basic_mat = am->get_material("basic");
    txt = new texture();
    image* img = am->get_image("diffuse");
    txt->init(img->get_width(), img->get_height(), img->get_data());
    norm_txt = new texture();
    img = am->get_image("brick");
    norm_txt->init(img->get_width(), img->get_height(), img->get_data());
    basic_mat->set_property_value("ambient_texture", txt);
    basic_mat->set_property_value("ambient_texture_strength", .8f);
    basic_mat->set_property_value("normal_texture", norm_txt);
    basic_mat->set_property_value("light_pos", 0.0f, 10.0f, 5.0f);
    basic_mat->set_property_value("light_color", 1.0f, 0.9f, 0.8f);
    basic_mat->set_property_value("light_intensity", 1.0f);

    _view_cameras[ 0 ]->get_transform().set_position({ 100, 0, 0 });
    _view_cameras[ 0 ]->get_transform().set_rotation(glm::quatLookAt(
        glm::vec3 { 1.0f, 0.0f, 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f }));

    _view_cameras[ 1 ]->get_transform().set_position({ 0, 100, 0 });
    _view_cameras[ 1 ]->get_transform().set_rotation(glm::quatLookAt(
        glm::vec3 { 0.0f, 1.0f, 0.0f }, glm::vec3 { 0.0f, 0.0f, 1.0f }));

    _view_cameras[ 2 ]->get_transform().set_position({ 0, 0, 100 });
    _view_cameras[ 2 ]->get_transform().set_rotation(glm::quatLookAt(
        glm::vec3 { 0.0f, 0.0f, 1.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f }));

    game_object* object = new game_object;
    object->create_component<mesh_component>();
    object->create_component<mesh_renderer_component>();
    object->create_component<jumpy_component>();
    object->get_component<mesh_component>()->set_mesh(am->meshes()[ 2 ]);
    object->get_component<mesh_renderer_component>()->set_material(basic_mat);
    object->set_name("susane");
    s.add_object(object);

    game_object* ray = new game_object;
    cast_ray = ray->create_component<ray_visualize_component>();
    ray->set_name("cast_ray");
    s.add_object(ray);
    cast_ray->set_ray({ 0, 0, 0 }, { 0, 0, 1 });

    main_camera_object = new game_object();
    main_camera_object->create_component<camera_component>();
    main_camera_object->get_component<camera_component>()->set_camera(
        main_camera);
    main_camera_object->set_name("main_camera");
    s.add_object(main_camera_object);

    ttf.load("font.ttf", 30);
    _fps_text_object = new game_object;
    _fps_text_object->create_component<text_component>();
    _fps_text_object->create_component<text_renderer_component>();
    _fps_text_object->create_component<fps_show_component>();
    _fps_text_object->get_component<text_renderer_component>()->set_font(&ttf);
    _fps_text_object->get_component<text_renderer_component>()->set_material(
        am->get_material("text"));
    am->get_material("text")->set_property_value("textColor", 1.0f, 1.0f, 1.0f);
    _fps_text_object->get_transform().set_position({ 0.5f, 2.0f, 0.0f });
    _fps_text_object->get_transform().set_scale({ 0.01f, 0.01f, 1.0f });
    _fps_text_object->set_name("fps_text");
    s.add_object(_fps_text_object);

    main_camera->get_transform().set_position({ 0, 0, 10 });
    main_camera->get_transform().set_rotation(
        glm::quatLookAt(glm::vec3 { 0.0f, 0.0f, 1.0f },
                        glm::vec3 {
                            0.0f,
                            1.0f,
                            0.0f,
                        }));
    main_camera_object->get_transform() = main_camera->get_transform();
    main_camera->set_ortho(false);
}
