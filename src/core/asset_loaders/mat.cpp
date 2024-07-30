#include <nlohmann/json.hpp>
#include <png.h>

#include "core/asset_loaders/mat.hpp"

#include "common/file.hpp"
#include "common/logging.hpp"
#include "core/asset_manager.hpp"
#include "graphics/material.hpp"

namespace
{
static inline logger log() { return get_logger("asset_loader_mat"); }
} // namespace

using json = nlohmann::json;

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
    std::string content = file::read_all(path);
    json mat_struct = json::parse(content);

    std::string shader_exclusive_name =
        mat_struct[ "shader" ].get<std::string>();
    auto* am = asset_manager::default_asset_manager();
    shader_program* sh;
    std::filesystem::path dir = std::filesystem::path(path).parent_path();
    std::string shader_name = (dir / shader_exclusive_name).string();
    std::string shader_path = shader_name + ".shader";

    if (sh = am->get_shader(shader_name); !sh)
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

    _material = new material;
    _material->set_shader_program(sh);

    for (auto& prop : mat_struct[ "properties" ])
    {
        auto prop_type = material_property::data_type::unknown;
        auto prop_name = prop[ "name" ].get<std::string>();

        if (prop.contains("type"))
        {
            if (prop[ "type" ].get<std::string>() == "image")
            {
                prop_type = material_property::data_type::type_image;
            }
            else if (prop[ "type" ].get<std::string>() == "float")
            {
                prop_type = material_property::data_type::type_float;
            }
            else if (prop[ "type" ].get<std::string>() == "vec4")
            {
                prop_type = material_property::data_type::type_float_vector_4;
            }
        }

        _material->declare_property(prop_name, prop_type);
        if (prop.contains("value"))
        {
            switch (prop_type)
            {
            case material_property::data_type::type_float:
            {
                _material->set_property_value(
                    prop_name,
                    details::from_json<float>(prop[ "value" ],
                                              std::make_index_sequence<1>()));
                break;
            }
            case material_property::data_type::type_float_vector_4:
            {
                _material->set_property_value(
                    prop_name,
                    details::from_json<float, float, float, float>(
                        prop[ "value" ], std::make_index_sequence<4>()));
                break;
            }
            default:
            {
                // TODO: prop_type enum can be stringified
                log()->warn("Parsing of data type \"{}\" is not supported",
                            static_cast<int>(prop_type));
                break;
            }
            }
        }
    }
}

material* asset_loader_MAT::get_material() { return _material; }
