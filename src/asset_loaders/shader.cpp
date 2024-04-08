#include "asset_loaders/shader.hpp"

#include "../shader.hpp"
#include "file.hpp"

void asset_loader_SHADER::load(std::string_view path)
{
    std::string content = file::read_all(path);
    std::stringstream ss(content);
    std::string shader_line;
    shader_program* prog = new shader_program();
    prog->init();
    while (std::getline(ss, shader_line, '\n') && file_exists(shader_line))
    {
        prog->add_shader(shader_line);
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
