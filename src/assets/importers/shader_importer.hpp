#pragma once

#include "assets/type_importer.hpp"

namespace assets
{
class shader_importer : public assets::type_importer<graphics::shader>
{
public:
    static constexpr std::string_view extensions { ".shader" };
    void internal_load(common::file& asset_file) override;
    void internal_update(asset_data_t sh, common::file& asset_file) override;

private:
    void load_shader(common::file& asset_file);
    void load_shader_script(common::file& asset_file);
};
} // namespace assets
