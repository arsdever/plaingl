#include "core/asset_loaders/shader.hpp"

#include "graphics/shader.hpp"

void asset_loader_SHADER::load(std::string_view path)
{
    _shader = graphics::shader::from_file(path);
}

std::shared_ptr<graphics::shader> asset_loader_SHADER::get_shader()
{
    return _shader;
}
