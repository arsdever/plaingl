#include "renderer_2d.hpp"

#include "core/asset_manager.hpp"
// #include "camera.hpp"
#include "experimental/viewport.hpp"
#include "glad/gl.h"
#include "graphics/shader.hpp"
#include "graphics/vaomap.hpp"
#include "graphics/vertex.hpp"
#include "graphics_buffer.hpp"
#include "renderer/algorithms/polygon_to_mesh.hpp"

void renderer_2d::draw_rect(glm::vec2 top_left,
                            glm::vec2 bottom_right,
                            float border_thickness,
                            glm::vec4 border_color,
                            glm::vec4 fill_color)
{
    std::vector<glm::vec2> points;
    std::vector<colored_vertex2d> vertices;
    std::vector<unsigned> indices;

    glm::vec2 bottom_left { top_left.x, bottom_right.y };
    glm::vec2 top_right { bottom_right.x, top_left.y };
    points = {
        top_left,
        bottom_left,
        bottom_right,
        top_right,
    };

    for (const auto& point : points)
    {
        colored_vertex2d v;
        v.position() = point;
        v.color() = fill_color;
        vertices.push_back(v);
    }

    indices = { 0, 1, 2, 0, 2, 3 };
    unsigned index_offset = vertices.size();

    polygon_to_mesh(
        points,
        true,
        border_thickness,
        [ border_color, &vertices ](glm::vec2 v)
    {
        colored_vertex2d vert;
        vert.position() = v;
        vert.color() = border_color;
        vertices.push_back(vert);
    },
        [ index_offset, &indices ](unsigned i)
    { indices.push_back(i + index_offset); });

    vao_map vao;
    graphics_buffer vbo(graphics_buffer::type::vertex);
    graphics_buffer ebo(graphics_buffer::type::index);

    vbo.set_element_stride(colored_vertex2d::size);
    vbo.set_element_count(vertices.size());
    ebo.set_element_stride(sizeof(unsigned));
    ebo.set_element_count(indices.size());
    vbo.set_data(vertices.data());
    ebo.set_data(indices.data());

    if (vao.activate())
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo.get_handle());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.get_handle());
        colored_vertex2d::initialize_attributes();
    }

    glDisable(GL_DEPTH_TEST);
    colored_vertex2d::activate_attributes();

    auto shader_2d =
        asset_manager::default_asset_manager()->get_shader("canvas");

    glm::uvec2 usize = experimental::viewport::current_viewport()->get_size();
    shader_2d->set_uniform("u_view_dimensions", usize);
    shader_2d->set_uniform("u_color", fill_color);
    shader_2d->use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    shader_2d->set_uniform("u_color", border_color);
    shader_2d->use();
    glDrawElements(GL_TRIANGLES,
                   indices.size() - 6,
                   GL_UNSIGNED_INT,
                   (void*)(6 * sizeof(unsigned)));

    glEnable(GL_DEPTH_TEST);
}
