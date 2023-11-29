/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <array>
#include <atomic>
#include <charconv>
#include <chrono>
#include <thread>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_window.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "text.hpp"
#include "thread.hpp"

namespace
{
static logger log() { return get_logger("main"); }
shader_program prog;
unsigned last_fps;
unsigned vao;
unsigned vbo;
unsigned ebo;
text console_text;
std::string console_text_content;
std::unordered_set<int> pressed_keys;
material basic_mat;
} // namespace

void process_console();
void on_keypress(
    GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
    {
        return;
    }

    switch (key)
    {
    case GLFW_KEY_ENTER:
    {
        process_console();
        break;
    }
    case GLFW_KEY_ESCAPE:
    {
        console_text_content.clear();
        break;
    }
    case GLFW_KEY_BACKSPACE:
    {
        if (!console_text_content.empty())
            console_text_content.pop_back();
        break;
    }
    default:
    {
        if (key >= GLFW_KEY_SPACE && key <= GLFW_KEY_GRAVE_ACCENT)
        {
            if (isalnum(key))
            {
                if (mods & GLFW_MOD_SHIFT)
                {
                    console_text_content.push_back(toupper(key));
                }
                else
                {
                    console_text_content.push_back(tolower(key));
                }
            }
            else
            {
                console_text_content.push_back(key);
            }
        }
    }
    }

    console_text.set_text(console_text_content);
}

std::vector<std::string_view> tokenize(std::string_view str);

void initScene();

static std::atomic_int counter = 0;

int main(int argc, char** argv)
{
    glfwInit();

    std::vector<gl_window> windows;
    windows.push_back({});
    windows.back().init();
    windows.push_back({});
    windows.back().init();

    initScene();

    console_text_content = "Hello world";
    console_text.set_text(console_text_content);

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
    set_thread_name(thd, "fps_counter");

    while (true)
    {
        for (auto& window : windows)
        {
            window.update();
        }
        glfwPollEvents();
    }

    color col { 1, 1, 1, 1 };
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

    basic_mat.set_shader_program(&prog);
    for (const auto& property : basic_mat.properties())
    {
        log()->info("Material properties:\n\tname: {}\n\tindex: {}\n\tsize: "
                    "{}\n\ttype: {}",
                    property._name,
                    property._index,
                    property._size,
                    property._type);
    }
    basic_mat.set_property("position", 1);

    {
        shader_program text_prog;
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
        console_text.set_shader(std::move(text_prog));
    }

    font console_text_font;
    console_text_font.load("font.ttf");

    console_text.init();
    console_text.set_font(std::move(console_text_font));
    console_text.set_color({ 0, 0, 0 });
    console_text.set_position({ 0, 570 });
    console_text.set_scale(.5f);

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

// void draw()
// {
//     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT);
//     basic_mat.activate();
//     glBindVertexArray(vao);
//     glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
//     glBindVertexArray(0);
//     prog.unuse();

//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     console_text.render();
// }

std::vector<std::string_view> tokenize(std::string_view str)
{
    std::vector<std::string_view> result;
    const char* iter = str.data();
    size_t length = 0;
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[ i ] == ' ')
        {
            result.push_back({ iter, length });
            iter += length + 1;
            length = 0;
            continue;
        }
        length++;
    }

    result.push_back({ iter, length });
    return result;
}

void process_console()
{
    auto tokens = tokenize(console_text_content);
    if (tokens[ 0 ] == "set" && tokens[ 1 ] == "position")
    {
        float x, y;
        std::from_chars(
            tokens[ 2 ].data(), tokens[ 2 ].data() + tokens[ 2 ].size(), x);
        std::from_chars(
            tokens[ 3 ].data(), tokens[ 3 ].data() + tokens[ 3 ].size(), y);

        console_text.set_position({ x, y });
    }
}
