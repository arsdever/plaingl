/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <array>
#include <atomic>
#include <charconv>
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
#include "color.hpp"
#include "components/mesh_component.hpp"
#include "components/mesh_renderer_component.hpp"
#include "components/text_component.hpp"
#include "components/text_renderer_component.hpp"
#include "font.hpp"
#include "game_object.hpp"
#include "gizmo_object.hpp"
#include "gl_window.hpp"
#include "image.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mouse_events_refiner.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "texture_viewer.hpp"
#include "thread.hpp"

namespace
{
static logger log() { return get_logger("main"); }
unsigned last_fps;
font ttf;
scene s;
std::unordered_set<int> pressed_keys;
camera main_camera;
camera second_camera;
mouse_events_refiner mouse_events;
game_object* _fps_text_object;
texture* txt;
texture* norm_txt;
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
    glfwInit();
    glfwSetErrorCallback(on_error);

    auto* am = asset_manager::default_asset_manager();
    am->load_asset("sample.png");
    am->load_asset("sample_jpg.jpg");
    texture_viewer::show_preview(am->get_image("sample_jpg"));

    std::vector<gl_window*> windows;
    windows.push_back(new gl_window);
    windows.back()->init();
    windows.back()->set_active();
    windows.back()->set_camera(&main_camera);
    windows.back()->on_window_closed += [ &windows ](gl_window* window)
    { windows.erase(std::find(windows.begin(), windows.end(), window)); };

    game_object* selected_object = nullptr;

    windows.back()->set_mouse_events_refiner(&mouse_events);
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
        from.y = params._window->height() - from.y;
        to.y = params._window->height() - to.y;
        log()->trace("dragging started from position ({}, {})", from.x, from.y);
        s.gizmo_objects()[ 0 ]->_line = { from, to };
    };
    mouse_events.drag_drop_move +=
        [](mouse_events_refiner::mouse_event_params params)
    {
        glm::vec2 to = params._position;
        to.y = params._window->height() - to.y;
        log()->trace("dragging to position ({}, {})", to.x, to.y);
        s.gizmo_objects()[ 0 ]->_line.value()[ 1 ] = to;
    };
    mouse_events.drag_drop_end +=
        [](mouse_events_refiner::mouse_event_params params)
    { s.gizmo_objects()[ 0 ]->_line = {}; };

    initScene();

    s.gizmo_objects().back()->_line = { { { -1, -1 }, { .5, .5 } } };

    windows.push_back(new gl_window);
    windows.back()->init();
    windows.back()->set_camera(&second_camera);
    windows.back()->on_window_closed += [ &windows ](gl_window* window)
    { windows.erase(std::find(windows.begin(), windows.end(), window)); };

    // windows.front()->toggle_indexing();
    // windows.back()->toggle_indexing();

    // fps counter thread
    logger fps_counter_log = get_logger("fps_counter");
    std::atomic_bool program_exits = false;
    std::thread thd { [ &fps_counter_log, &program_exits ]
    {
        while (!program_exits)
        {
            fps_counter_log->info("FPS: {}", counter);
            last_fps = counter;
            counter = 0;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } };
    set_thread_name(thd, "fps_counter");

    while (!windows.empty())
    {
        for (int i = 0; i < windows.size(); ++i)
        {
            auto p = prof::profile_frame(__FUNCTION__);
            auto window = windows[ i ];
            window->set_active();
            // color c { 0, 0, 0 };
            double timed_fraction =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            // hslToRgb(fmod(timed_fraction, 5.0) / 5.0, 1.0f, .5f, c.r, c.g,
            // c.b); for (auto& obj : s.objects())
            // {
            //     if (obj->get_component<mesh_renderer_component>())
            //     {
            //         obj->get_component<mesh_renderer_component>()
            //             ->get_material()
            //             ->set_property_value(
            //                 "materialColor",
            //                 std::tuple<float, float, float, float> {
            //                     c.r, c.g, c.b, 1.0f });
            //     }
            // }

            main_camera.get_transform().set_position(
                { sin(timed_fraction) * 10, 0, cos(timed_fraction) * 10 });

            _fps_text_object->get_component<text_component>()->set_text(
                fmt::format(
                    "{:#6.6} ms\nhandle {:#x}",
                    // std::chrono::duration_cast<std::chrono::duration<double>>(
                    //     diff)
                    //         .count() *
                    //     1000,
                    16.146,
                    reinterpret_cast<unsigned long long>(window)));

            window->update();
        }
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
    am->load_asset("text.mat");
    am->load_asset("basic.mat");
    am->load_asset("sample.png");
    am->load_asset("brick.png");

    material* basic_mat = am->get_material("basic");
    txt = new texture();
    image* img = am->get_image("sample");
    txt->init(img->get_width(), img->get_height(), img->get_data());
    norm_txt = new texture();
    img = am->get_image("brick");
    norm_txt->init(img->get_width(), img->get_height(), img->get_data());
    basic_mat->set_property_value("ambient_texture", txt);
    basic_mat->set_property_value("normal_texture", norm_txt);

    game_object* object = new game_object;
    object->create_component<mesh_component>();
    object->create_component<mesh_renderer_component>();
    object->get_component<mesh_component>()->set_mesh(am->meshes()[ 0 ]);
    object->get_component<mesh_renderer_component>()->set_material(basic_mat);
    object->get_transform().set_position({ -0.5f, 0, 0 });

    s.add_object(object);

    object = new game_object;
    object->create_component<mesh_component>();
    object->create_component<mesh_renderer_component>();
    object->get_component<mesh_component>()->set_mesh(am->meshes()[ 1 ]);
    object->get_component<mesh_renderer_component>()->set_material(basic_mat);
    object->get_transform().set_position({ 0.5f, 0, 0 });

    s.add_object(object);

    ttf.load("font.ttf", 12);
    _fps_text_object = new game_object;
    _fps_text_object->create_component<text_component>();
    _fps_text_object->create_component<text_renderer_component>();
    _fps_text_object->get_component<text_renderer_component>()->set_font(&ttf);
    _fps_text_object->get_component<text_renderer_component>()->set_material(
        am->get_material("text"));
    am->get_material("text")->set_property_value("textColor", 1.0f, 1.0f, 1.0f);
    _fps_text_object->get_transform().set_position({ 0.5f, 2.0f, 0 });

    s.add_object(_fps_text_object);

    s.add_gizmo_object(new gizmo_object);

    main_camera.get_transform().set_position({ 10, 10, 10 });
    second_camera.get_transform().set_position({ 0, -10, 10 });
}
