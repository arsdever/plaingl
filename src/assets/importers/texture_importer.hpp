#pragma once

#include "assets/type_importer.hpp"

namespace assets
{
class texture_importer : public assets::type_importer<graphics::texture>
{
public:
    static constexpr std::string_view extensions { ".png,.jpg,.jpeg" };
    void initialize_asset(asset& ast) override;
    void read_asset_data(std::string_view asset_path) override;
};
} // namespace assets
