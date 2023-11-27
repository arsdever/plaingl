#include <array>
#include <atomic>
#include <map>
#include <thread>

/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "logging.hpp"
#include "shader.hpp"
#include "text.hpp"

namespace
{
static logger log() { return get_logger("main"); }
shader_program prog;
shader_program text_prog;
unsigned last_fps;
unsigned vao;
unsigned vbo;
unsigned ebo;
text console_text;
} // namespace

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void initScene();
void draw();

static std::atomic_int counter = 0;

int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        log()->error("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        log()->error("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    initScene();

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

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        draw();
        ++counter;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    program_exits = true;
    glfwTerminate();
    thd.join();
    return 0;
}

void initScene()
{
    prog.init();
    prog.add_shader("shader.vert");
    prog.add_shader("shader.frag");
    prog.link();

    text_prog.init();
    text_prog.add_shader("text.vert");
    text_prog.add_shader("text.frag");
    text_prog.link();

    text_prog.use();
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    unsigned int uniform_position =
        glGetUniformLocation(text_prog.id(), "projection");
    glUniformMatrix4fv(
        uniform_position, 1, GL_FALSE, glm::value_ptr(projection));
    text_prog.unuse();

    font console_text_font;
    console_text_font.load("font.ttf");

    console_text.init();
    console_text.set_shader(std::move(text_prog));
    console_text.set_font(std::move(console_text_font));
    console_text.set_color({ 0, 0, 0 });
    console_text.set_position({ 0, 570 });
    console_text.set_scale(.5f);
    console_text.set_text("Hello world");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    std::array<std::array<float, 3>, 3> vertices {
        { { -.5, -.5, 0 }, { 0, .75, 0 }, { .5, -.5, 0 } },
    };

    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * vertices[ 0 ].size() *
                     sizeof(vertices[ 0 ][ 0 ]),
                 vertices.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::array<int, 3> indices { 0, 1, 2 };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices),
                 indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void draw()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    prog.use();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    prog.unuse();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    console_text.render();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
