#pragma once

#include "asset_management/type_importer.hpp"

class font_importer : public assets::type_importer<font>
{
public:
    static constexpr std::string_view extensions { ".ttf" };
    void internal_load(common::file& asset_file) override;
};
