#include "core/asset_loaders/shader.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "graphics/shader.hpp"

namespace fs = common::filesystem;

void asset_loader_SHADER::load(std::string_view path)
{
    std::string content = common::file::read_all(path);
    std::stringstream ss(content);
    std::string shader_line;
    shader_program* prog = new shader_program();
    prog->init();
    while (std::getline(ss, shader_line, '\n'))
    {
        auto shader_path =
            (fs::path(fs::path(path).full_path_without_filename()) /
             shader_line);
        if (common::file::exists(shader_path.full_path()))
        {
            prog->add_shader(shader_path.full_path());
        }
    }

    prog->link();
    if (prog->linked())
    {
        _shader_program = prog;
    }
}

shader_program* asset_loader_SHADER::get_shader_program()
{
    return _shader_program;
}
