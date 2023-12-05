#include "asset_manager.hpp"

#include "asset_loaders/fbx.hpp"

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
}

const std::vector<mesh*>& asset_manager::meshes() const { return _meshes; }
