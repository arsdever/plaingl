/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include "graphics/graphics.hpp"

#include "common/logging.hpp"
#include "graphics/gl_error_handler.hpp"

namespace graphics
{
namespace
{
static inline logger log() { return get_logger("graphics"); }
} // namespace
bool initialize()
{
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        log()->error("Failed to initialize GLAD");

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_error_handler, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    return true;
}

void clear(glm::dvec4 background)
{
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void set_viewport(glm::vec2 top_left, glm::vec2 bottom_right)
{
    glm::vec2 bottom_left = { top_left.x, bottom_right.y };
    glViewport(bottom_left.x,
               top_left.y,
               std::abs(bottom_right.x - bottom_left.x),
               std::abs(bottom_right.y - top_left.y));
}
} // namespace graphics
