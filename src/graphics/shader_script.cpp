#include <glad/gl.h>

#include "graphics/graphics_fwd.hpp"

#include "graphics/shader_script.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "common/utils.hpp"

namespace graphics
{
namespace
{
static inline logger log() { return get_logger("shader"); }
} // namespace

template <>
int gl_convert<int, graphics::shader_script_type>(
    graphics::shader_script_type st)
{
    switch (st)
    {
    case graphics::shader_script_type::vertex: return GL_VERTEX_SHADER;
    case graphics::shader_script_type::fragment: return GL_FRAGMENT_SHADER;
    case graphics::shader_script_type::compute: return GL_COMPUTE_SHADER;
    default: return -1;
    }
}

shader_script::shader_script(std::string_view path)
    : shader_script(std::string(path), get_type_from_path(path))
{
}

shader_script::shader_script(std::string_view path, shader_script_type type)
    : shader_script(std::string(path), type)
{
}

shader_script::shader_script(std::string path, shader_script_type type)
    : _type(type)
    , _path(std::move(path))
    , _content(common::file::read_all(_path))
    , _id(glCreateShader(gl_convert<int>(type)))
{
}

void shader_script::update(std::string_view content) { _content = content; }

void shader_script::compile()
{
    auto src = _content.c_str();
    glShaderSource(_id, 1, &src, nullptr);
    glCompileShader(_id);

    int error_code;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &error_code);
    if (error_code == GL_FALSE)
    {
        int log_length = 0;
        char msg[ 1024 ];
        glGetShaderInfoLog(_id, 1024, &log_length, msg);
        log()->error("Failed to compile shader {}({}): {}", _id, _path, msg);
        return;
    }

    _compiled = true;
}

int shader_script::id() const { return _id; }

shader_script_type shader_script::get_type_from_path(std::string_view path)
{
    common::filesystem::path p(path);
    auto ext = p.extension();
    if (ext == ".vert")
    {
        return shader_script_type::vertex;
    }
    else if (ext == ".frag")
    {
        return shader_script_type::fragment;
    }
    else if (ext == ".comp")
    {
        return shader_script_type::compute;
    }
    else
    {
        return shader_script_type::unknown;
    }
}

shader_script shader_script::from_file(std::string_view path)
{
    return shader_script(path);
}
} // namespace graphics
