#pragma once

#include "assets/type_importer.hpp"

namespace assets
{
class font_importer : public assets::type_importer<graphics::font>
{
public:
    static constexpr std::string_view extensions { ".ttf" };
    void internal_load(common::file& asset_file) override;
};
} // namespace assets
