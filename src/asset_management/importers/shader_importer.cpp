#include "asset_management/importers/shader_importer.hpp"

#include "asset_management/asset_importer.hpp"
#include "common/logging.hpp"
#include "graphics/shader.hpp"
#include "graphics/shader_script.hpp"

namespace
{
logger log() { return get_logger("asset_manager"); }
} // namespace

void shader_importer::internal_load(common::file& asset_file)
{
    load_shader(asset_file);
}

void shader_importer::load_shader(common::file& asset_file)
{
    _data = std::make_shared<graphics::shader>();

    if (!asset_file.exists())
    {
        log()->error("Shader script {} does not exist",
                     asset_file.get_filepath());
        return;
    }

    std::string content = asset_file.read_all();
    std::vector<std::string> shader_script_paths;

    // tokenize content by newline characters
    std::string::size_type start = 0;
    auto spath = common::filesystem::path(asset_file.get_filepath());

    auto new_start = content.find('\n', start);
    while (new_start != std::string::npos)
    {
        auto line = content.substr(start, new_start - start);
        shader_script_paths.push_back(line);
        start = new_start + 1;
        new_start = content.find('\n', start);
        std::shared_ptr<graphics::shader_script> ss =
            std::make_shared<graphics::shader_script>(
                (common::filesystem::path(spath.full_path_without_filename()) /
                 line)
                    .full_path());
        _data->add_shader(ss);
    }

    _data->set_name(std::string(spath.stem()));
    _data->compile();
}
