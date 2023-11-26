/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <map>

#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include FT_FREETYPE_H
#include <array>
#include <atomic>
#include <iostream>
#include <thread>

#include "logging.hpp"
#include "shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
static logger log() { return get_logger("main"); }
shader_program prog;
shader_program text_prog;
unsigned last_fps;
unsigned vao;
unsigned vbo;
unsigned ebo;

unsigned tvao;
unsigned tvbo;

struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    long Advance;           // Offset to advance to next glyph
};

std::map<char, Character> Characters;
} // namespace

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void initFont();
void initScene();
void draw();
void render_text(shader_program& prog,
                 std::string text,
                 float x,
                 float y,
                 float scale,
                 float r,
                 float g,
                 float b);
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
    initFont();

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

void initFont()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        log()->error("FREETYPE: Could not init FreeType Library");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "font.ttf", 0, &face))
    {
        log()->error("FREETYPE: Failed to load font");
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        log()->error("FREETYTPE: Failed to load Glyph");
        return;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT,
                  1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            log()->error("FREETYTPE: Failed to load Glyph");
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            { face->glyph->bitmap.width, face->glyph->bitmap.rows },
            { face->glyph->bitmap_left, face->glyph->bitmap_top },
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void render_text(shader_program& prog,
                 std::string text,
                 float x,
                 float y,
                 float scale,
                 float r,
                 float g,
                 float b)
{
    // activate corresponding render state
    prog.use();
    glUniform3f(glGetUniformLocation(prog.id(), "textColor"), r, g, b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(tvao);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[ *c ];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[ 6 ][ 4 ] = { { xpos, ypos + h, 0.0f, 0.0f },
                                     { xpos, ypos, 0.0f, 1.0f },
                                     { xpos + w, ypos, 1.0f, 1.0f },

                                     { xpos, ypos + h, 0.0f, 0.0f },
                                     { xpos + w, ypos, 1.0f, 1.0f },
                                     { xpos + w, ypos + h, 1.0f, 0.0f } };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, tvbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    glGenVertexArrays(1, &tvao);
    glGenBuffers(1, &tvbo);
    glBindVertexArray(tvao);
    glBindBuffer(GL_ARRAY_BUFFER, tvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
    render_text(text_prog, fmt::format("FPS: {}", last_fps), 0, 0, 0.7f, 0, 0, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
