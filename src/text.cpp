#include "text.hpp"

#include "font.hpp"
#include "glad/gl.h"

text::text() = default;

void text::init()
{
    glGenVertexArrays(1, &tvao);
    glGenBuffers(1, &tvbo);
    glBindVertexArray(tvao);
    glBindBuffer(GL_ARRAY_BUFFER, tvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void text::set_font(font font) { _font = font; }

void text::set_text(std::string text) { _text = std::move(text); }

void text::set_shader(shader_program prog) { _shader = std::move(prog); }

void text::set_color(color text_color) { _color = text_color; }

void text::set_position(position text_position) { _position = text_position; }

void text::set_scale(float scale) { _scale = scale; }

void text::render() const
{
    _shader.use();
    glUniform3f(glGetUniformLocation(_shader.id(), "textColor"),
                _color.r,
                _color.g,
                _color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(tvao);

    // iterate through all characters
    std::string::const_iterator c;
    position cursor_position = _position;
    for (c = _text.begin(); c != _text.end(); c++)
    {
        const font::character& ch = _font[ *c ];

        float xpos = cursor_position.x + ch._bearing.x * _scale;
        float ypos = cursor_position.y - (ch._size.y - ch._bearing.y) * _scale;

        float w = ch._size.x * _scale;
        float h = ch._size.y * _scale;
        // update VBO for each character
        float vertices[ 6 ][ 4 ] = { { xpos, ypos + h, 0.0f, 0.0f },
                                     { xpos, ypos, 0.0f, 1.0f },
                                     { xpos + w, ypos, 1.0f, 1.0f },

                                     { xpos, ypos + h, 0.0f, 0.0f },
                                     { xpos + w, ypos, 1.0f, 1.0f },
                                     { xpos + w, ypos + h, 1.0f, 0.0f } };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch._texture_id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, tvbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        cursor_position.x +=
            (ch._advance >> 6) *
            _scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    _shader.unuse();
}

const shader_program& text::get_shader() const { return _shader; }

shader_program& text::get_shader() { return _shader; }
