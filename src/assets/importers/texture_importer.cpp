#include "assets/importers/texture_importer.hpp"

#include "common/main_thread_dispatcher.hpp"
#include "graphics/texture.hpp"

void texture_importer::internal_load(common::file& asset_file)
{
    _data = graphics::texture::from_file(asset_file);
}

void texture_importer::internal_update(std::shared_ptr<graphics::texture> txt,
                                       common::file& f)
{
    txt->set_contents_from_file(f);
}
