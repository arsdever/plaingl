#include <nlohmann/json.hpp>

#include "asset_management/importers/material_importer.hpp"

#include "asset_management/asset.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "graphics/material.hpp"

using json = nlohmann::json;
namespace fs = common::filesystem;

namespace
{
static inline logger log() { return get_logger("asset_manager"); }

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
} // namespace

void material_importer::internal_load(common::file& asset_file)
{
    _data = std::make_shared<graphics::material>();

    std::string content = asset_file.read_all();
    json mat_struct = json::parse(content);

    std::string shader_exclusive_name =
        mat_struct[ "shader" ].get<std::string>();
    std::string shader_path(
        (fs::path(
             fs::path(asset_file.get_filepath()).full_path_without_filename()) /
             shader_exclusive_name +
         ".shader")
            .full_path());

    auto sh =
        core::asset_manager::try_get<graphics::shader>(shader_exclusive_name);

    if (!sh)
    {
        core::asset_manager::get_importer().import(
            shader_path, core::asset_manager::get_cache());
    }

    if (sh = core::asset_manager::try_get<graphics::shader>(
            shader_exclusive_name);
        !sh)
    {
        log()->error(
            "(Shader file '{}' required by material '{}' could not be found) ",
            shader_path,
            asset_file.get_filepath());
        return;
    }

    _data->set_shader_program(sh->as<graphics::shader>());

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
                _data->set_property_value(
                    prop_name,
                    details::from_json<float>(prop[ "value" ],
                                              std::make_index_sequence<1>()));
                break;
            }
            case 4:
            {
                _data->set_property_value(
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
