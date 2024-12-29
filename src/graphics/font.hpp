#pragma once

#include "common/common_fwd.hpp"
#include "graphics/graphics_fwd.hpp"

namespace graphics
{
class font
{
public:
    struct character
    {
        // position in texture atlas
        glm::uvec2 _texture_offset;
        // size of glyph
        glm::uvec2 _size;
        // offset from baseline to left/top of glyph
        glm::uvec2 _bearing;
        // offset to advance to next glyph
        float _advance;
    };

public:
    font();

    void load(std::string path, float size);
    void load(common::file& f, float size);
    std::shared_ptr<graphics::texture> atlas();

    glm::vec2 size(std::string_view msg) const;

    const character& operator[](unsigned int ch) const;

private:
    // TODO: later split into multiple logical parts:
    //       1. graph
    //       2. generic packing
    struct font_node
    {
        glm::uvec2 _origin { 0 };
        glm::uvec2 _size { 0 };
        int _id { -1 };
        std::shared_ptr<font_node> _l { nullptr };
        std::shared_ptr<font_node> _r { nullptr };

        font_node* insert(int ch, const glm::uvec2& font_size);

        static glm::uvec2 texture_size;
    };

private:
    std::string _font_file_path;
    std::map<unsigned int, character> _character_map;
    std::shared_ptr<graphics::texture> _atlas;
};
} // namespace graphics
