/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include "graphics/vaomap.hpp"

#include "graphics/vertex.hpp"

vao_map::~vao_map()
{
    auto ctx = glfwGetCurrentContext();
    for (auto& [ context, vao ] : _context_dependent_map)
    {
        glfwMakeContextCurrent(context);
        glDeleteVertexArrays(1, &vao);
    }
    glfwMakeContextCurrent(ctx);
}

bool vao_map::activate()
{
    if (_context_dependent_map.contains(glfwGetCurrentContext()))
    {
        glBindVertexArray(_context_dependent_map[ glfwGetCurrentContext() ]);
        return false;
    }

    _context_dependent_map[ glfwGetCurrentContext() ] = 0;
    glGenVertexArrays(1, &_context_dependent_map[ glfwGetCurrentContext() ]);
    glBindVertexArray(_context_dependent_map[ glfwGetCurrentContext() ]);
    return true;
}
