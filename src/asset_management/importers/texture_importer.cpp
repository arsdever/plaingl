#include "asset_management/importers/texture_importer.hpp"

#include "graphics/texture.hpp"

void texture_importer::internal_load(common::file& asset_file)
{
    _data = graphics::texture::from_file(asset_file);
}
