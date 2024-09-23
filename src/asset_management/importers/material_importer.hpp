#pragma once

#include "asset_management/assets_fwd.hpp"

#include "asset_management/type_importer.hpp"

class material_importer : public assets::type_importer<graphics::material>
{
public:
    static constexpr std::string_view extensions { ".mat" };
    void internal_load(common::file& asset_file) override;
    void internal_update(asset_data_t mat, common::file& asset_file) override;
};
