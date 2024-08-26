#include <glad/gl.h>
#include <prof/profiler.hpp>

#include "renderer/renderer_3d.hpp"

#include "core/asset_manager.hpp"
#include "glad/gl.h"
#include "graphics/graphics_buffer.hpp"
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
    mat->activate();

    glDrawElements(GL_TRIANGLES,
                   m->get_index_buffer().get_element_count(),
                   GL_UNSIGNED_INT,
                   0);
}
