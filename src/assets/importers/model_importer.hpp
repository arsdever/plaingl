#pragma once

#include "graphics/graphics_fwd.hpp"

#include "assets/type_importer.hpp"

namespace assets
{
class model_importer : public assets::type_importer<graphics::mesh>
{
public:
    static constexpr std::string_view extensions { ".fbx" };
    void initialize_asset(asset& ast) override;
    void read_asset_data(std::string_view asset_path) override;
};
} // namespace assets
