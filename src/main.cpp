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
#include "game_object.hpp"
#include "gizmo_object.hpp"
#include "gl_window.hpp"
#include "image.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "mouse_events_refiner.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "text.hpp"
#include "texture_viewer.hpp"
#include "thread.hpp"

namespace
{
static logger log() { return get_logger("main"); }
unsigned last_fps;
text console_text;
scene s;
std::string console_text_content;
std::unordered_set<int> pressed_keys;
camera main_camera;
camera second_camera;
mouse_events_refiner mouse_events;
} // namespace

void process_console();
void on_keypress(
    GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        return;
    }

    switch (key)
    {
    case GLFW_KEY_ENTER:
    {
        process_console();
        break;
    }
    case GLFW_KEY_ESCAPE:
    {
        console_text_content.clear();
        break;
    }
    case GLFW_KEY_BACKSPACE:
    {
        if (!console_text_content.empty())
            console_text_content.pop_back();
        break;
    }
    default:
    {
        if (key >= GLFW_KEY_SPACE && key <= GLFW_KEY_GRAVE_ACCENT)
        {
            if (isalnum(key))
            {
                if (mods & GLFW_MOD_SHIFT)
                {
                    console_text_content.push_back(toupper(key));
                }
                else
                {
                    console_text_content.push_back(tolower(key));
                }
            }
            else
            {
                console_text_content.push_back(key);
            }
        }
    }
    }

    console_text.set_text(console_text_content);
}

std::vector<std::string_view> tokenize(std::string_view str);

void initScene();

void on_error(int error_code, const char* description)
{
    log()->error("Error {}: {}", error_code, description);
}

static std::atomic_int counter = 0;

int main(int argc, char** argv)
{
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
        glm::vec2 size = { params._window->width(), params._window->height() };
        glm::vec2 half_size = size / 2.0f;
        glm::vec2 from = (params._old_position - half_size) / size * 2.0f;
        glm::vec2 to = (params._position - half_size) / size * 2.0f;
        from.y = -from.y;
        to.y = -to.y;
        log()->trace("dragging started from position ({}, {})", from.x, from.y);
        s.gizmo_objects()[ 0 ]->_line = { from, to };
    };
    mouse_events.drag_drop_move +=
        [](mouse_events_refiner::mouse_event_params params)
    {
        glm::vec2 size = { params._window->width(), params._window->height() };
        glm::vec2 half_size = size / 2.0f;
        glm::vec2 to = (params._position - half_size) / size * 2.0f;
        to.y = -to.y;
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

    console_text_content = "Hello world";
    console_text.set_text(console_text_content);

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
            color c { 0, 0, 0 };
            double timed_fraction =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    std::chrono::steady_clock::now().time_since_epoch())
                    .count();
            hslToRgb(fmod(timed_fraction, 5.0) / 5.0, 1.0f, .5f, c.r, c.g, c.b);
            s.objects().back()->get_material()->set_property_value(
                "materialColor",
                std::tuple<float, float, float, float> { c.r, c.g, c.b, 1.0f });

            main_camera.get_transform().set_position(
                { sin(timed_fraction) * 10, 0, cos(timed_fraction) * 10 });
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

    material* basic_mat = new material;
    basic_mat->set_shader_program(prog);
    basic_mat->declare_property(
        "materialColor", material_property::data_type::type_float_vector_4);
    basic_mat->declare_property("is_selected",
                                material_property::data_type::type_integer);
    basic_mat->declare_property("model_matrix",
                                material_property::data_type::unknown);
    // for (const auto& property : basic_mat->properties())
    // {
    //     log()->info("Material properties:\n\tname: {}\n\tindex: {}\n\tsize: "
    //                 "{}\n\ttype: {}",
    //                 property._name,
    //                 property._index,
    //                 property._size,
    //                 property._type);
    // }
    basic_mat->set_property_value("position", 1);

    auto* am = asset_manager::default_asset_manager();
    am->load_asset("cube.fbx");
    am->load_asset("sphere.fbx");

    game_object* object = new game_object;
    object->set_mesh(am->meshes()[ 0 ]);
    object->set_material(basic_mat);
    object->get_transform().set_position({ -0.5f, 0, 0 });

    s.add_object(object);

    object = new game_object;
    object->set_mesh(am->meshes()[ 1 ]);
    object->set_material(basic_mat);
    object->get_transform().set_position({ 0.5f, 0, 0 });

    s.add_object(object);

    s.add_gizmo_object(new gizmo_object);

    main_camera.get_transform().set_position({ 10, 10, 10 });
    second_camera.get_transform().set_position({ 0, -10, 10 });
}

std::vector<std::string_view> tokenize(std::string_view str)
{
    std::vector<std::string_view> result;
    const char* iter = str.data();
    size_t length = 0;
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[ i ] == ' ')
        {
            result.push_back({ iter, length });
            iter += length + 1;
            length = 0;
            continue;
        }
        length++;
    }

    result.push_back({ iter, length });
    return result;
}

void process_console()
{
    auto tokens = tokenize(console_text_content);
    if (tokens[ 0 ] == "set" && tokens[ 1 ] == "position")
    {
        float x, y;
        std::from_chars(
            tokens[ 2 ].data(), tokens[ 2 ].data() + tokens[ 2 ].size(), x);
        std::from_chars(
            tokens[ 3 ].data(), tokens[ 3 ].data() + tokens[ 3 ].size(), y);

        console_text.set_position({ x, y });
    }
}
