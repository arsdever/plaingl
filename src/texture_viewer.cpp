/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <array>

#include "texture_viewer.hpp"

#include "image.hpp"
#include "logging.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace
{
inline logger log() { return get_logger("texture_viewer"); }
static inline void gl_debug_output(GLenum source,
                                   GLenum type,
                                   unsigned int id,
                                   GLenum severity,
                                   GLsizei length,
                                   const char* message,
                                   const void* userParam)
{
    // SOURCE: https://learnopengl.com/In-Practice/Debugging
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    log()->error("---------------");
    log()->error("Debug message ({}): {}", id, message);

    switch (source)
    {
    case GL_DEBUG_SOURCE_API: log()->error("Source: API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        log()->error("Source: Window System");
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        log()->error("Source: Shader Compiler");
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        log()->error("Source: Third Party");
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        log()->error("Source: Application");
        break;
    case GL_DEBUG_SOURCE_OTHER: log()->error("Source: Other"); break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: log()->error("Type: Error"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        log()->error("Type: Deprecated Behaviour");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        log()->error("Type: Undefined Behaviour");
        break;
    case GL_DEBUG_TYPE_PORTABILITY: log()->error("Type: Portability"); break;
    case GL_DEBUG_TYPE_PERFORMANCE: log()->error("Type: Performance"); break;
    case GL_DEBUG_TYPE_MARKER: log()->error("Type: Marker"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP: log()->error("Type: Push Group"); break;
    case GL_DEBUG_TYPE_POP_GROUP: log()->error("Type: Pop Group"); break;
    case GL_DEBUG_TYPE_OTHER: log()->error("Type: Other"); break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: log()->error("Severity: high"); break;
    case GL_DEBUG_SEVERITY_MEDIUM: log()->error("Severity: medium"); break;
    case GL_DEBUG_SEVERITY_LOW: log()->error("Severity: low"); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        log()->error("Severity: notification");
        break;
    }
}
} // namespace

void texture_viewer::show_preview(texture* t)
{
    auto* window = glfwCreateWindow(t->get_width(),
                                    t->get_height(),
                                    "texture preview",
                                    nullptr,
                                    glfwGetCurrentContext());
    if (window == nullptr)
    {
        log()->error("Failed to create GLFW window");
        return;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        log()->error("Failed to initialize GLAD");
        return;
    }

    {
        // configure gl debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glViewport(0, 0, t->get_width(), t->get_height());

    glfwSetWindowSizeCallback(window,
                              [](GLFWwindow*, int width, int height)
    { glViewport(0, 0, width, height); });

    shader_program prog;
    prog.init();
    prog.add_shader("texture.vert");
    prog.add_shader("texture.frag");
    prog.link();
    prog.use();

    std::array<std::array<float, 2>, 6> points {
        { { -1, -1 }, { -1, 1 }, { 1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } }
    };
    unsigned vao = 0;
    unsigned vbo = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 points.size() * sizeof(decltype(points)::value_type),
                 points.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    t->bind(0);

    while (!glfwWindowShouldClose(window))
    {
        glBindVertexArray(vao);
        prog.use();
        glUniform1i(glGetUniformLocation(prog.id(), "texture_sampler"), 0);
        glUniform1ui(glGetUniformLocation(prog.id(), "texture_type"), t->get_channel_count());
        glDrawArrays(GL_TRIANGLES, 0, points.size());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glBindVertexArray(0);
    shader_program::unuse();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwDestroyWindow(window);
}

void texture_viewer::show_preview(image* img)
{
    texture t = texture::from_image(img);
    show_preview(&t);
}
