#include "renderer_2d.hpp"

#include "asset_manager.hpp"
#include "glad/gl.h"
#include "graphics_buffer.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "vaomap.hpp"
#include "vertex.hpp"

void renderer_2d::draw_rect(glm::vec2 top_left,
                            glm::vec2 bottom_right,
                            float border_thickness,
                            glm::vec4 border_color,
                            glm::vec4 fill_color)
{
    struct colored_vertex_2d : vertex<position_2d_attribute, color_attribute>
    {
        position_2d_attribute::attribute_data_storage_type& position()
        {
            return get<1>();
        }

        color_attribute::attribute_data_storage_type& color()
        {
            return get<0>();
        }
    };

    std::vector<glm::vec2> points;
    std::vector<colored_vertex_2d> vertices;
    std::vector<unsigned> indices;

    glm::vec2 bottom_left { top_left.x, bottom_right.y };
    glm::vec2 top_right { bottom_right.x, top_left.y };
    points = { top_left, top_right, bottom_right, bottom_left };

    for (auto& point : points)
    {
        colored_vertex_2d v;
        v.position() = point;
        v.color() = fill_color;
        vertices.push_back(v);
    }

    indices = { 0, 1, 1, 2, 2, 3, 3, 0 };

    vao_map vao;
    graphics_buffer vbo(graphics_buffer::type::vertex);
    graphics_buffer ebo(graphics_buffer::type::index);

    vbo.set_element_stride(colored_vertex_2d::size);
    vbo.set_element_count(vertices.size());
    ebo.set_element_stride(sizeof(unsigned));
    ebo.set_element_count(indices.size());
    vbo.set_data(vertices.data());
    ebo.set_data(indices.data());

    if (vao.activate())
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo.get_handle());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.get_handle());
        colored_vertex_2d::initialize_attributes();
    }

    auto shader_2d =
        asset_manager::default_asset_manager()->get_shader("2d_rendering");

    colored_vertex_2d::activate_attributes();

    shader_2d->use();
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glEnable(GL_DEPTH_TEST);
}
