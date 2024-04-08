#include <freetype/freetype.h>

#include "font.hpp"

#include "glad/gl.h"
#include "logging.hpp"

namespace
{
static logger log() { return get_logger("font"); }
} // namespace

font::font() = default;

void font::load(std::string path, float size)
{
    _font_file_path = std::move(path);
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

    FT_Set_Pixel_Sizes(face, 0, size);
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
        // TODO: use our texture class when it supports custom color types
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
        character ch = { texture,
                         { face->glyph->bitmap.width,
                           face->glyph->bitmap.rows },
                         { face->glyph->bitmap_left, face->glyph->bitmap_top },
                         face->glyph->advance.x };
        _character_map.insert(std::pair<char, character>(c, ch));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

const font::character& font::operator[](char ch) const
{
    return _character_map.at(ch);
}
