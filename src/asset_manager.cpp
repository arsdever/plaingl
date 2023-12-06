#include "asset_manager.hpp"

#include "asset_loaders/fbx.hpp"
#include "asset_loaders/png.hpp"

void asset_manager::load_asset(std::string_view path)
{
    std::string_view extension = path.substr(path.find_last_of("."));
    if (extension == ".fbx")
    {
        asset_loader_FBX fbx_loader;
        fbx_loader.load(path);
        _meshes.insert(_meshes.end(),
                       fbx_loader.get_meshes().begin(),
                       fbx_loader.get_meshes().end());
    }
    else if (extension == ".png")
    {
        asset_loader_PNG png_loader;
        png_loader.load(path);
        _textures.push_back(png_loader.get_image());
    }
}

const std::vector<mesh*>& asset_manager::meshes() const { return _meshes; }

const std::vector<image*>& asset_manager::textures() const { return _textures; }
