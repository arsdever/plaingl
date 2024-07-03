#include <prof/profiler.hpp>

#include "renderer/renderer_3d.hpp"

#include "core/asset_manager.hpp"
// #include "camera.hpp"
#include "experimental/viewport.hpp"
#include "glad/gl.h"
#include "graphics/command.hpp"
#include "graphics/commands/draw_elements.hpp"
#include "graphics/graphics_buffer.hpp"
#include "graphics/graphics_channel.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/vaomap.hpp"
#include "graphics/vertex.hpp"

void renderer_3d::draw_mesh(mesh* m, material* mat)
{
    auto sp = prof::profile(__FUNCTION__);
    if (_vao.activate())
    {
        glBindBuffer(GL_ARRAY_BUFFER, m->get_vertex_buffer().get_handle());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     m->get_index_buffer().get_handle());
        vertex3d::initialize_attributes();
    }

    vertex3d::activate_attributes();

    // if (auto cam = camera::active_camera())
    // {
    //     mat->set_property_value("u_vp_matrix", cam->vp_matrix());
    //     mat->set_property_value("u_camera_position",
    //                             cam->get_transform().get_position());
    // }

    mat->activate();

    auto cmd =
        graphics::command { graphics::command_type::draw_elements,
                            graphics::draw_elements_args {
                                GL_TRIANGLES,
                                m->get_index_buffer().get_element_count(),
                                GL_UNSIGNED_INT,
                                0 } };

    graphics::graphics_channel::instance().post(std::move(cmd));
    graphics::graphics_channel::instance().flush();
}
