#pragma once

#include "assets/type_importer.hpp"

namespace assets
{
class font_importer : public assets::type_importer<graphics::font>
{
public:
    static constexpr std::string_view extensions { ".ttf" };
    void initialize_asset(asset& ast) override;
    void read_asset_data(std::string_view asset_path) override;
};
} // namespace assets
