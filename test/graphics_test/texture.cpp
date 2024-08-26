#include <array>

#include "texture.hpp"

#include "core/asset_manager.hpp"
#include "core/window.hpp"
#include "graphics_buffer.hpp"
#include "shader.hpp"
#include "vaomap.hpp"
#include "vertex.hpp"

std::array<graphics_buffer, 2> init_quad_mesh()
{
    std::array<vertex3d, 4> quad_vertices;
    quad_vertices[ 0 ].position() = { -1, -1, 0 };
    quad_vertices[ 1 ].position() = { 1, -1, 0 };
    quad_vertices[ 2 ].position() = { 1, 1, 0 };
    quad_vertices[ 3 ].position() = { -1, 1, 0 };

    quad_vertices[ 0 ].uv() = { -1, -1 };
    quad_vertices[ 1 ].uv() = { 2, -1 };
    quad_vertices[ 2 ].uv() = { 2, 2 };
    quad_vertices[ 3 ].uv() = { -1, 2 };

    std::array<unsigned, 6> quad_indices = { 0, 1, 2, 0, 2, 3 };

    graphics_buffer vbo(graphics_buffer::type::vertex);
    graphics_buffer ebo(graphics_buffer::type::index);

    vbo.set_element_count(4);
    vbo.set_element_stride(vertex3d::size);
    vbo.set_data(quad_vertices.data());

    ebo.set_element_count(6);
    ebo.set_element_stride(sizeof(unsigned));
    ebo.set_data(quad_indices.data());
    return { std::move(vbo), std::move(ebo) };
}

int main(int argc, char** argv)
{
    glfwInit();
    std::shared_ptr<core::window> wnd_interpolation =
        std::make_shared<core::window>();
    std::shared_ptr<core::window> wnd_repeat = std::make_shared<core::window>();

    wnd_interpolation->set_title("Sampling mode visualization");
    wnd_interpolation->resize(400, 200);
    wnd_interpolation->init();

    wnd_repeat->set_title("Repeat mode visualization");
    wnd_repeat->resize(400, 400);
    wnd_repeat->init();

    asset_manager::default_asset_manager()->load_asset(
        "resources/standard/surface.shader");
    asset_manager::default_asset_manager()->load_asset(
        "test/graphics_test/data/parrot_pixel.png");

    auto txt = texture::from_image(
        asset_manager::default_asset_manager()->get_image("parrot_pixel"));

    auto [ vbo, ebo ] = init_quad_mesh();

    wnd_repeat->get_events()->render += [ &vbo, &ebo, &txt ](const auto& e)
    {
        vao_map vao;
        if (vao.activate())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo.get_handle());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.get_handle());
            vertex3d::initialize_attributes();
        }

        vertex3d::activate_attributes();

        auto* surface_shader =
            asset_manager::default_asset_manager()->get_shader("surface");

        txt.bind();
        txt.set_active_texture(0);

        surface_shader->set_uniform("u_image", 0);
        surface_shader->use();

        txt.set_sampling_mode_mag(texture::sampling_mode::nearest);

        static const std::array<std::function<void()>, 4> modes {
            [ &txt ]()
        { txt.set_wrapping_mode(true, true, texture::wrapping_mode::repeat); },
            [ &txt ]()
        {
            txt.set_wrapping_mode(
                false, true, texture::wrapping_mode::clamp_to_border);
        },
            [ &txt ]() {
            txt.set_wrapping_mode(
                true, true, texture::wrapping_mode::clamp_to_edge);
        },
            [ &txt ]() {
            txt.set_wrapping_mode(
                true, true, texture::wrapping_mode::mirrored_repeat);
        },
        };

        size_t mode_selector =
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count() %
            modes.size();

        modes[ mode_selector ]();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, 400, 400);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    };

    wnd_interpolation->get_events()->render += [ &vbo, &ebo, &txt ](const auto&)
    {
        vao_map vao;
        if (vao.activate())
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbo.get_handle());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.get_handle());
            vertex3d::initialize_attributes();
        }

        vertex3d::activate_attributes();

        auto* surface_shader =
            asset_manager::default_asset_manager()->get_shader("surface");

        txt.bind();
        txt.set_active_texture(0);

        surface_shader->set_uniform("u_image", 0);
        surface_shader->use();

        txt.set_wrapping_mode(true, true, texture::wrapping_mode::repeat);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        txt.set_sampling_mode_mag(texture::sampling_mode::nearest);
        glViewport(0, 0, 200, 200);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        txt.set_sampling_mode_mag(texture::sampling_mode::linear);
        glViewport(200, 0, 200, 200);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    };

    int should_close = 2;

    wnd_interpolation->get_events()->close +=
        [ &should_close ](auto) { --should_close; };
    wnd_repeat->get_events()->close +=
        [ &should_close ](auto) { --should_close; };

    while (should_close)
    {
        wnd_interpolation->update();
        wnd_repeat->update();
    }

    return 0;
}
