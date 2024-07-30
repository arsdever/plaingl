#include "core/asset_loaders/shader.hpp"

#include "common/file.hpp"
#include "graphics/shader.hpp"

void asset_loader_SHADER::load(std::string_view path)
{
    std::string content = file::read_all(path);
    std::stringstream ss(content);
    std::string shader_line;
    shader_program* prog = new shader_program();
    prog->init();
    while (std::getline(ss, shader_line, '\n'))
    {
        std::filesystem::path dir = std::filesystem::path(path).parent_path();
        std::string shader_name = (dir / shader_line).string();
        if (file::exists(shader_name))
        {
            prog->add_shader(shader_name);
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
