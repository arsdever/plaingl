/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include "components/text_renderer_component.hpp"

#include "components/text_component.hpp"
#include "font.hpp"
#include "logging.hpp"
#include "material.hpp"

namespace
{
static inline logger log()
{
    return get_logger(text_renderer_component::class_type_id);
}
} // namespace

text_renderer_component::text_renderer_component(game_object* parent)
    : component(parent, class_type_id)
{
    if (get_component<text_component>() == nullptr)
    {
        log()->error("requires mesh_component");
    }
}

void text_renderer_component::set_font(font* ttf) { _font = ttf; }

font* text_renderer_component::get_font() { return _font; }

void text_renderer_component::set_material(material* mat) { _material = mat; }

material* text_renderer_component::get_material() { return _material; }

void text_renderer_component::init()
{
    glGenBuffers(1, &tvbo);
    glBindBuffer(GL_ARRAY_BUFFER, tvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void text_renderer_component::update()
{
    if (_material && get_component<text_component>())
    {
        _material->set_property_value(
            "model_matrix", get_game_object()->get_transform().get_matrix());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _material->activate();
        glActiveTexture(GL_TEXTURE0);

        if (!_vao_map.contains(glfwGetCurrentContext()))
        {
            _vao_map[ glfwGetCurrentContext() ] = 0;
            glGenVertexArrays(1, &_vao_map[ glfwGetCurrentContext() ]);
            glBindVertexArray(_vao_map[ glfwGetCurrentContext() ]);
            glBindBuffer(GL_ARRAY_BUFFER, tvbo);
            glVertexAttribPointer(
                0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glEnableVertexAttribArray(0);
        }
        else
        {
            glBindVertexArray(_vao_map[ glfwGetCurrentContext() ]);
            glEnableVertexAttribArray(0);
        }

        // iterate through all characters
        std::string::const_iterator c;
        glm::vec2 scale =
            get_game_object()->get_transform().get_scale() / 20.0f;
        glm::vec2 cursor_position = { 0.0f, 0.0f };

        std::string_view text_str = get_component<text_component>()->get_text();
        for (auto& c : text_str)

        {
            const font::character& ch = (*_font)[ static_cast<size_t>(c) ];

            float xpos = cursor_position.x + ch._bearing.x * scale.x;
            float ypos =
                cursor_position.y - (ch._size.y - ch._bearing.y) * scale.y;

            float w = ch._size.x * scale.x;
            float h = ch._size.y * scale.y;
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
            // now advance cursors for next glyph (note that advance is number
            // of 1/64 pixels)
            cursor_position.x +=
                (ch._advance >> 6) *
                scale.x; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
        _material->deactivate();
    }

    // if (_material)
    // {
    //     _material->set_property_value(
    //         "model_matrix", get_game_object()->get_transform().get_matrix());
    //     _material->activate();

    //     if (get_component<text_component>())
    //     {
    //         get_component<text_component>()->get_mesh()->render();
    //     }
    //     _material->deactivate();
    // }
}

void text_renderer_component::deinit()
{
    for (auto& [ _, tvao ] : _vao_map)
    {
        glDeleteVertexArrays(1, &tvao);
    }
    glDeleteBuffers(1, &tvbo);
}
