#include "core/asset_loaders/shader.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "graphics/shader.hpp"

namespace fs = common::filesystem;

void asset_loader_SHADER::load(std::string_view path)
{
    _shader = new graphics::shader();
    *_shader = std::move(graphics::shader::from_file(path));
}

graphics::shader* asset_loader_SHADER::get_shader() { return _shader; }
