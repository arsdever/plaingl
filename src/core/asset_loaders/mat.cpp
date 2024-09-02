#include <nlohmann/json.hpp>
#include <png.h>

#include "core/asset_loaders/mat.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "core/asset_manager.hpp"
#include "graphics/material.hpp"

namespace
{
static inline logger log() { return get_logger("asset_loader_mat"); }
} // namespace

using json = nlohmann::json;
namespace fs = common::filesystem;

namespace details
{
template <typename... Args, std::size_t... Is>
std::tuple<Args...> from_json(const nlohmann::json& args,
                              std::index_sequence<Is...>)
{
    if constexpr (std::tuple_size_v<std::tuple<Args...>> == 1)
    {
        return { args.get<Args>()... };
    }
    else
    {
        return { args[ Is ].get<Args>()... };
    }
}
} // namespace details

void asset_loader_MAT::load(std::string_view path)
{
    std::string content = common::file::read_all(path);
    json mat_struct = json::parse(content);

    std::string shader_exclusive_name =
        mat_struct[ "shader" ].get<std::string>();
    auto* am = asset_manager::default_asset_manager();
    std::shared_ptr<graphics::shader> sh;
    std::string shader_path(
        (fs::path(fs::path(path).full_path_without_filename()) /
             shader_exclusive_name +
         ".shader")
            .full_path());

    if (sh = am->get_shader(shader_exclusive_name); !sh)
    {
        am->load_asset(shader_path);
    }

    if (sh = am->get_shader(shader_exclusive_name); !sh)
    {
        log()->error(
            "(Shader file '{}' required by material '{}' could not be found) ",
            shader_path,
            path);
        return;
    }

    _material = std::make_shared<graphics::material>();
    _material->set_shader_program(sh);

    for (auto& prop : mat_struct[ "properties" ])
    {
        auto prop_name = prop[ "name" ].get<std::string>();
        if (prop.contains("value"))
        {
            // TODO: rework needed here as the type was erased from material
            // properties
            switch (prop[ "value" ].size())
            {
            case 1:
            {
                _material->set_property_value(
                    prop_name,
                    details::from_json<float>(prop[ "value" ],
                                              std::make_index_sequence<1>()));
                break;
            }
            case 4:
            {
                _material->set_property_value(
                    prop_name,
                    details::from_json<float, float, float, float>(
                        prop[ "value" ], std::make_index_sequence<4>()));
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }
}

std::shared_ptr<graphics::material> asset_loader_MAT::get_material()
{
    return _material;
}
