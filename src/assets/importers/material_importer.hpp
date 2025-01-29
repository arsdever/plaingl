#pragma once

#include "assets/assets_fwd.hpp"

#include "assets/type_importer.hpp"

namespace assets
{
class material_importer : public assets::type_importer<graphics::material>
{
public:
    static constexpr std::string_view extensions { ".mat" };
    void internal_load(common::file& asset_file) override;
    void internal_update(asset_data_t mat, common::file& asset_file) override;
};
}
