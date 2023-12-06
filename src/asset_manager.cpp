#include "asset_manager.hpp"

#include "asset_loaders/fbx.hpp"
#include "asset_loaders/mat.hpp"
#include "asset_loaders/png.hpp"
#include "asset_loaders/shader.hpp"

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
    else if (extension == ".shader")
    {
        asset_loader_SHADER shader_loader;
        shader_loader.load(path);
        _shaders.push_back(shader_loader.get_shader_program());
    }
    else if (extension == ".mat")
    {
        asset_loader_MAT mat_loader;
        mat_loader.load(path);
        _materials.push_back(mat_loader.get_material());
    }
    else if (extension == ".png")
    {
        asset_loader_PNG png_loader;
        png_loader.load(path);
        _textures.push_back(png_loader.get_image());
    }
}

const std::vector<mesh*>& asset_manager::meshes() const { return _meshes; }

const std::vector<material*>& asset_manager::materials() const
{
    return _materials;
}

const std::vector<image*>& asset_manager::textures() const { return _textures; }


asset_manager* asset_manager::default_asset_manager()
{
    if (_instance == nullptr)
    {
        _instance = new asset_manager;
    }

    return _instance;
}

asset_manager* asset_manager::_instance = nullptr;
