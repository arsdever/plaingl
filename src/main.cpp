#include <GLFW/glfw3.h>
#include <prof/profiler.hpp>
#include <spdlog/common.h>

#include "core/asset_manager.hpp"
#include "core/command_dispatcher.hpp"
#include "core/input_system.hpp"
#include "core/window.hpp"
#include "core/window_events.hpp"
#include "feature_flags.hpp"
#include "file.hpp"
#include "font.hpp"
#include "game_clock.hpp"
#include "graphics/gpu.hpp"
#include "image.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "project/components/camera.hpp"
#include "project/components/component_registry.hpp"
#include "project/components/light.hpp"
#include "project/components/mesh_filter.hpp"
#include "project/components/mesh_renderer.hpp"
#include "project/components/transform.hpp"
#include "project/game_object.hpp"
#include "project/project_commands.hpp"
#include "renderer/renderer_2d.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "thread.hpp"
#include "tools/console/console.hpp"
#include "tools/mesh_viewer/mesh_viewer.hpp"
#include "tools/profiler/profiler.hpp"
#include "tools/texture_viewer/texture_viewer.hpp"

using namespace core;

namespace
{
static logger log() { return get_logger("main"); }
unsigned last_fps;
font ttf;
std::shared_ptr<scene> s;
std::unordered_set<int> pressed_keys;
std::shared_ptr<game_object> main_camera_object;
game_object* _fps_text_object;
texture* txt;
texture* norm_txt;
// ray_visualize_component* cast_ray;
std::vector<std::shared_ptr<core::window>> windows;
int mesh_preview_texture_index { 0 };
std::unique_ptr<console> pconsole;

class cmd_show_mesh : public core::command<int>
{
public:
    using command::command;
    void execute() override
    {
        int num = get<0>();
        if (num >= 0 &&
            num < asset_manager::default_asset_manager()->meshes().size())
        {
            auto mv = std::make_shared<mesh_viewer>();
            mv->init();
            mv->set_mesh(
                asset_manager::default_asset_manager()->meshes()[ num ]);
            mv->get_events()->close += [](auto e)
            {
                windows.erase(std::remove(windows.begin(),
                                          windows.end(),
                                          e.get_sender()->shared_from_this()));
            };
            windows.push_back(mv);
        }
    }
};

class cmd_show_texture : public core::command<int>
{
public:
    using command::command;
    void execute() override
    {
        int num = get<0>();
        if (num >= 0 &&
            num < asset_manager::default_asset_manager()->textures().size())
        {
            auto tv = std::make_shared<texture_viewer>();
            tv->init();
            tv->set_texture(texture::_textures[ num ]);
            tv->get_events()->close += [](auto e)
            {
                windows.erase(std::remove(windows.begin(),
                                          windows.end(),
                                          e.get_sender()->shared_from_this()));
            };
            windows.push_back(tv);
        }
    }
};

class cmd_list_textures : public core::command<>
{
public:
    void execute() override
    {
        for (int i = 0; i < texture::_textures.size(); ++i)
        {
            log()->info(
                "Texture {}: id {}", i, texture::_textures[ i ]->native_id());
        }
    }
};

// physics_engine p;
} // namespace

void load_internal_resources();
void initScene();
void initMainWindow();
void initProfilerView();
void setupMouseEvents();

void on_error(int error_code, const char* description)
{
    log()->error("Error {}: {}", error_code, description);
}

static std::atomic_int counter = 0;

int main(int argc, char** argv)
{
    // register components
    component_registry::register_components();
    // components::box_collider::register_component();
    // components::plane_collider::register_component();
    // components::walking::register_component();
    // components::light::register_component();
    // components::text_renderer::register_component();
    // components::text::register_component();
    // components::camera::register_component();
    // components::fps_show::register_component();
    // components::ray_visualize::register_component();

    configure_logging(argc, argv);
    game_clock* clock = game_clock::init();
    glfwInit();
    glfwSetErrorCallback(on_error);
    adjust_timeout_accuracy_guard guard;

    game_object* selected_object = nullptr;

    initMainWindow();
    // initProfilerView();
    setupMouseEvents();
    initScene();

    core::input_system::on_keypress += [](int keycode)
    {
        if (keycode == GLFW_KEY_GRAVE_ACCENT)
        {
            if (pconsole->is_active())
            {
                pconsole->deactivate();
            }
            else
            {
                pconsole->activate();
            }
        }
        else if (pconsole->is_active())
        {
            pconsole->input(keycode);
        }
    };

    pconsole = std::make_unique<console>();
    pconsole->register_for_logs();
    pconsole->register_command<cmd_show_texture, int>("show.texture");
    pconsole->register_command<cmd_show_mesh, int>("show.mesh");
    pconsole->register_command<cmd_list_textures>("list.textures");
    pconsole->register_command<project::cmd_create_game_object>(
        "create.gameobject");
    pconsole->register_command<project::cmd_print_selected_object>(
        "show.selected");
    pconsole->register_command<project::cmd_rename_object, std::string>(
        "rename");
    pconsole->register_command<project::cmd_select_object, size_t>("select");
    pconsole->register_command<project::cmd_list_objects>("list.objects");

    log()->info("Using graphics card {} - {}",
                graphics::gpu::get_vendor(),
                graphics::gpu::get_device());

    // TODO: may not be the best place for object initialization
    // Probably should be done in some sort of scene loading procedure
    if (scene::get_active_scene())
    {
        scene::get_active_scene()->visit_root_objects([](auto obj)
        { obj->init(); });
    }

    common::file mat_file("resources/standard/standard.mat");
    mat_file.changed += [](common::file_change_type et)
    { log()->info("material file changed"); };

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
    auto camera = components::camera::get_active();
    // camera->set_background(glm::dvec4 { 1.0, 0.0, 0.0, 0.0 });
    // texture* txt = new texture;
    // main_camera->set_render_texture(txt);

    texture* txt_show = nullptr;
    bool console_mode = false;

    auto profview = std::make_shared<profiler>();
    profview->init();
    profview->get_events()->close += [](auto ce)
    {
        windows.erase(std::remove(windows.begin(),
                                  windows.end(),
                                  ce.get_sender()->shared_from_this()));
    };
    windows.push_back(profview);

    // auto wh = file::watch("./",
    //                       [](auto path, auto change)
    // { log()->info("Path {} changed: {}", path, static_cast<int>(change)); });

    while (!windows.empty())
    {
        {
            auto p = prof::profile_frame(__FUNCTION__);
            scene::get_active_scene()->visit_root_objects([](auto obj)
            { obj->update(); });

            for (int i = 0; i < windows.size(); ++i)
            {
                auto window = windows[ i ];
                window->update();
            }
        }
        clock->frame();
        pconsole->processor().execute_all();

        if (txt_show != 0)
        {
            // texture_viewer::show_preview(txt_show);
            txt_show = nullptr;
        }
    }

    std::stringstream ss;
    ss << std::this_thread::get_id();
    prof::apply_data(ss.str(),
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

void initMainWindow()
{
    windows.push_back(std::make_shared<core::window>());
    auto& wnd = windows.back();
    wnd->on_user_initialize += [](auto) { load_internal_resources(); };
    wnd->init();
    wnd->resize(800, 800);
    wnd->get_events()->close += [](auto ce)
    { std::erase(windows, ce.get_sender()->shared_from_this()); };
    wnd->get_events()->resize += [](auto re)
    { components::camera::get_active()->set_render_size(re.get_new_size()); };
    wnd->get_events()->render += [](auto re)
    {
        components::camera::get_active()->render();

        float scale = 1.0f;
        const auto line_height = 18.0f * scale;
        auto baseline = line_height + 5;
        auto yoffset = 0.0f;
        if (pconsole->is_active())
        {
            renderer_2d().draw_rect(
                { 0, 0 },
                { re.get_sender()->get_width(), line_height * 11 + 10 },
                { re.get_sender()->get_width(), re.get_sender()->get_height() },
                0,
                glm::vec4(0),
                glm::vec4(0));
            yoffset = line_height * 11 + 10;
            auto history = pconsole->history<10>();
            for (int i = 0; i < history.size(); ++i)
            {
                auto& text = history[ history.size() - i - 1 ];
                if (!text.empty())
                {
                    renderer_2d().draw_text({ 5, baseline },
                                            ttf,
                                            { re.get_sender()->get_width(),
                                              re.get_sender()->get_height() },
                                            text,
                                            scale);
                }
                baseline += line_height;
            }
            std::string p = std::format("> {}", pconsole->get_current_input());
            renderer_2d().draw_text(
                { 5, baseline },
                ttf,
                { re.get_sender()->get_width(), re.get_sender()->get_height() },
                p,
                scale);
            baseline += 2 * line_height;
        }

        if (auto obj = project::selected_object(); obj != nullptr)
        {
            renderer_2d().draw_rect(
                { 0, yoffset },
                { re.get_sender()->get_width(),
                  line_height * 5 + 10 + yoffset },
                { re.get_sender()->get_width(), re.get_sender()->get_height() },
                0,
                glm::vec4(0),
                glm::vec4(0));

            std::string p = std::format("{:10}: {}", "name", obj->get_name());
            renderer_2d().draw_text(
                { 5, baseline },
                ttf,
                { re.get_sender()->get_width(), re.get_sender()->get_height() },
                p,
                scale);

            baseline += line_height;
            if (auto gobj = std::dynamic_pointer_cast<game_object>(obj);
                gobj != nullptr)
            {
                p = std::format(
                    "{:10}: {}",
                    "position",
                    glm::to_string(gobj->get_transform().get_position()));
                renderer_2d().draw_text({ 5, baseline },
                                        ttf,
                                        { re.get_sender()->get_width(),
                                          re.get_sender()->get_height() },
                                        p,
                                        scale);
                baseline += line_height;
                p = std::format(
                    "{:10}: {}",
                    "rotation",
                    glm::to_string(gobj->get_transform().get_rotation()));
                renderer_2d().draw_text({ 5, baseline },
                                        ttf,
                                        { re.get_sender()->get_width(),
                                          re.get_sender()->get_height() },
                                        p,
                                        scale);
                baseline += line_height;
                p = std::format(
                    "{:10}: {}",
                    "scale",
                    glm::to_string(gobj->get_transform().get_scale()));
                renderer_2d().draw_text({ 5, baseline },
                                        ttf,
                                        { re.get_sender()->get_width(),
                                          re.get_sender()->get_height() },
                                        p,
                                        scale);
            }
        }
    };
    wnd->set_can_grab(true);
}

void setupMouseEvents()
{
    core::window::get_main_window()->get_events()->mouse_move += [](auto me)
    {
        if (me.get_sender()->get_has_grab())
        {
            main_camera_object->get_transform().set_rotation(
                glm::quat(glm::radians(glm::vec3(-me.get_local_position().y,
                                                 -me.get_local_position().x,
                                                 0) *
                                       .1f)));
        }
        else
        {
            auto cam = components::camera::get_active();
            // draw ray casted from camera
            auto pos = cam->get_transform().get_position();
            auto rot = cam->get_transform().get_rotation();
            glm::vec3 point =
                glm::unProject(glm::vec3 { me.get_local_position().x,
                                           me.get_sender()->get_height() -
                                               me.get_local_position().y,
                                           0 },
                               cam->view_matrix(),
                               cam->projection_matrix(),
                               glm::vec4 { 0,
                                           0,
                                           me.get_sender()->get_width(),
                                           me.get_sender()->get_height() });
            // cast_ray->set_ray(pos, glm::normalize(point - pos));

            // auto hit = p.raycast(pos, glm::normalize(point - pos));

            // if (hit.has_value())
            // {
            //     log()->info("hit collider");
            // }
        }
    };
}

void initScene()
{
    s = scene::create();
    feature_flags::set_flag(feature_flags::flag_name::load_fbx_as_scene, false);
    auto* am = asset_manager::default_asset_manager();
    material* basic_mat = am->get_material("standard");
    txt = new texture();
    image* img = am->get_image("albedo");
    *txt = std::move(texture::from_image(img));
    norm_txt = new texture();
    img = am->get_image("brick");
    *norm_txt = std::move(texture::from_image(img));
    auto roughness_txt = new texture;
    img = am->get_image("roughness");
    *roughness_txt = std::move(texture::from_image(img));
    auto metallic_txt = new texture;
    img = am->get_image("metallic");
    *metallic_txt = std::move(texture::from_image(img));
    basic_mat->set_property_value("u_albedo_texture", txt);
    basic_mat->set_property_value("u_albedo_texture_strength", 1.0f);
    basic_mat->set_property_value("u_normal_texture", norm_txt);
    basic_mat->set_property_value("u_normal_texture_strength", 1.0f);
    basic_mat->set_property_value("u_roughness", 1.0f);
    basic_mat->set_property_value("u_roughness_texture", roughness_txt);
    basic_mat->set_property_value("u_roughness_texture_strength", 1.0f);
    basic_mat->set_property_value("u_metallic", 1.0f);
    basic_mat->set_property_value("u_metallic_texture", metallic_txt);
    basic_mat->set_property_value("u_metallic_texture_strength", 1.0f);
    basic_mat->set_property_value("u_ao", 0.1f);

    auto object = game_object::create();
    object->add<components::mesh_filter>();

    object->get<components::mesh_filter>().set_mesh(
        am->get_mesh("Shader Ball JL 01_mesh"));
    object->add<components::mesh_renderer>().set_material(basic_mat);
    object->set_name("susane");
    s->add_root_object(object);
    // bc->set_position(glm::vec3(0, 0, 0));
    // bc->set_scale(glm::vec3(2, 1, 1));
    // bc->set_rotation(glm::quat(glm::ballRand(1.0f)));
    // bc->set_rotation(glm::quat(glm::radians(glm::vec3 { 0, 30, 0 })));

    ttf.load("font.ttf", 12);
    // game_object* collision_text_object = new game_object;
    // auto* ct = collision_text_object->create_component<text_component>();
    // collision_text_object->create_component<text_renderer_component>();
    // collision_text_object->get_component<text_renderer_component>()->set_font(
    //     &ttf);
    // collision_text_object->get_component<text_renderer_component>()
    //     ->set_material(am->get_material("text"));
    // s.add_object(collision_text_object);
    // bc->_text = ct;
    // collision_text_object->get_transform().set_scale({ 0.005f,
    // 0.005f, 1.0f
    // });

    // game_object* ray = new game_object;
    // cast_ray = ray->create_component<ray_visualize_component>();
    // ray->set_name("cast_ray");
    // s.add_object(ray);
    // cast_ray->set_ray({ 0, 0, 0 }, { 0, 0, 1 });

    main_camera_object = game_object::create();
    // main_camera_object->create_component<mesh_component>()->set_mesh(
    //     am->get_mesh("camera"));
    // main_camera_object->create_component<mesh_renderer_component>()
    //     ->set_material(am->get_material("standard"));
    auto& main_camera = main_camera_object->add<components::camera>();
    main_camera_object->set_name("main_camera");
    s->add_root_object(main_camera_object);

    // _fps_text_object = new game_object;
    // _fps_text_object->create_component<text_component>();
    // _fps_text_object->create_component<text_renderer_component>();
    // _fps_text_object->create_component<fps_show_component>();
    // _fps_text_object->get_component<text_renderer_component>()->set_font(&ttf);
    // _fps_text_object->get_component<text_renderer_component>()->set_material(
    //     am->get_material("text"));
    // am->get_material("text")->set_property_value(
    //     "u_text_color", 1.0f, 1.0f, 1.0f);
    // _fps_text_object->get_transform().set_position({ 0.5f, 2.0f, 0.0f });
    // _fps_text_object->get_transform().set_scale({ 0.01f, 0.01f, 1.0f });
    // _fps_text_object->set_name("fps_text");
    // s.add_object(_fps_text_object);

    main_camera_object->get_transform().set_position({ 0, 0, 3 });
    main_camera.get_transform().set_rotation(
        glm::quatLookAt(glm::vec3 { 0.0f, 0.0f, -1.0f },
                        glm::vec3 {
                            0.0f,
                            1.0f,
                            0.0f,
                        }));
    // main_camera_object->create_component<walking_component>();
    main_camera.set_orthogonal(false);
    main_camera.set_active();

    auto light_obj_1 = game_object::create();
    auto& l = light_obj_1->add<components::light>();
    l.set_color(glm::dvec4(1.0));
    l.set_intensity(10.0);
    // object = new game_object;
    // object->create_component<light_component>()->set_light(l);
    light_obj_1->get_transform().set_position(glm::vec3(0.0f, 1.5f, 0.0f));
    // s.add_object(object);
    light_obj_1->set_name("light_1");
    s->add_root_object(light_obj_1);

    auto light_obj_2 = game_object::create();
    auto& l2 = light_obj_2->add<components::light>();
    l2.set_color(glm::dvec4(1.0));
    l2.set_intensity(10.0);
    // object = new game_object;
    // object->create_component<light_component>()->set_light(l);
    light_obj_2->get_transform().set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    light_obj_2->set_name("light_2");
    s->add_root_object(light_obj_2);
}

void load_internal_resources()
{
    auto* am = asset_manager::default_asset_manager();
    am->load_asset("resources/internal/camera_background.shader");

    am->load_asset("resources/meshes/cube.fbx");
    am->load_asset("resources/meshes/sphere.fbx");
    am->load_asset("resources/meshes/env_sphere.fbx");
    am->load_asset("resources/meshes/susane_head.fbx");
    am->load_asset("resources/meshes/susane_head_low.fbx");
    am->load_asset("resources/meshes/shader_ball.fbx");
    am->load_asset("resources/meshes/camera.fbx");
    am->load_asset("resources/standard/skybox.mat");
    am->load_asset("resources/standard/surface.mat");
    am->load_asset("resources/standard/canvas.shader");
    am->load_asset("resources/standard/standard.mat");
    am->load_asset("resources/standard/mesh_viewer.mat");
    am->load_asset("resources/images/sample.png");
    am->load_asset("resources/images/brick.png");
    am->load_asset("resources/images/diffuse.png");
    am->load_asset("resources/images/albedo.jpg");
    am->load_asset("resources/images/metallic.jpg");
    am->load_asset("resources/images/roughness.jpg");
    am->load_asset("resources/images/env.jpg");
    am->load_asset("resources/images/white.png");
}

void initProfilerView()
{
    windows.push_back(std::make_shared<core::window>());
    auto& wnd = windows.back();
    wnd->init();
    wnd->resize(800, 200);
    wnd->get_events()->close += [](auto ce)
    { std::erase(windows, ce.get_sender()->shared_from_this()); };
    wnd->get_events()->render += [](auto re)
    {
        // prof::draw_data dd;
        // dd.height = re.get_sender()->get_height();
        // dd.width = re.get_sender()->get_width();
        // dd.zoom_x = .01f;
        // dd.zoom_y = 1.0f;
        // std::stringstream ss;
        // ss << std::this_thread::get_id();
        // prof::draw_overall_data(ss.str(), dd);
    };
    wnd->set_can_grab(false);
}
