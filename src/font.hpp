#pragma once

#include <map>
#include <string>

#include <glm/glm.hpp>

class font
{
public:
    struct character
    {
        unsigned int _texture_id; // ID handle of the glyph texture
        glm::ivec2 _size;         // size of glyph
        glm::ivec2 _bearing;      // offset from baseline to left/top of glyph
        long _advance;            // offset to advance to next glyph
    };

public:
    font();

    void load(std::string path);

    const character& operator[](char ch) const;

private:
    std::string _font_file_path;
    std::map<char, character> _character_map;
};
