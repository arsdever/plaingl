#include "assets/importers/font_importer.hpp"

#include "graphics/font.hpp"

void font_importer::internal_load(common::file& asset_file)
{
    _data = std::make_shared<graphics::font>();
    _data->load(asset_file, 12.0f);
}
