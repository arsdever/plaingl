#include <nlohmann/json.hpp>
#include <png.h>

#include "asset_loaders/mat.hpp"

#include "asset_manager.hpp"
#include "file.hpp"
#include "logging.hpp"
#include "material.hpp"

namespace
{
static inline logger log() { return get_logger("asset_loader_mat"); }
} // namespace

using json = nlohmann::json;

void asset_loader_MAT::load(std::string_view path)
{
    std::string content = get_file_contents(path);
    json mat_struct = json::parse(content);

    std::string shader_name = mat_struct[ "shader" ].get<std::string>();
    auto* am = asset_manager::default_asset_manager();
    shader_program* sh;

    if (sh = am->get_shader(shader_name); !sh)
    {
        am->load_asset(shader_name + ".shader");
    }

    if (sh = am->get_shader(shader_name); !sh)
    {
        log()->error(R"(Shader file "{}" required by material "{}" could not "
                     "be found)",
                     shader_name + ".shader",
                     path);
        return;
    }

    _material = new material;
    _material->set_shader_program(sh);

    for (auto& prop_name : mat_struct[ "properties" ])
    {
        _material->declare_property(prop_name.get<std::string>(),
                                    material_property::data_type::unknown);
    }
}

material* asset_loader_MAT::get_material() { return _material; }
