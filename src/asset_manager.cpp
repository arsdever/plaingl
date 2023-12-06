#include "asset_manager.hpp"

#include "asset_loaders/fbx.hpp"
#include "asset_loaders/mat.hpp"
#include "asset_loaders/png.hpp"
#include "asset_loaders/shader.hpp"
#include "file.hpp"
void asset_manager::load_asset(std::string_view path)
{
    auto [ _, filename, extension ] = parse_path(path);
    if (extension == ".fbx")
    {
        asset_loader_FBX fbx_loader;
        fbx_loader.load(path);
        auto [ it, success ] =
            _meshes.try_emplace(filename, fbx_loader.get_meshes());
    }
    else if (extension == ".shader")
    {
        asset_loader_SHADER shader_loader;
        shader_loader.load(path);
        auto [ it, success ] =
            _shaders.try_emplace(filename, shader_loader.get_shader_program());
    }
    else if (extension == ".mat")
    {
        asset_loader_MAT mat_loader;
        mat_loader.load(path);
        auto [ it, success ] =
            _materials.try_emplace(filename, mat_loader.get_material());
    }
    else if (extension == ".png")
    {
        asset_loader_PNG png_loader;
        png_loader.load(path);
        auto [ it, success ] =
            _textures.try_emplace(filename, png_loader.get_image());
    }
}

void asset_manager::update(std::string_view path)
{
    // TODO: can be improved
    auto [ _, filename, extension ] = parse_path(path);

    if (extension == ".fbx")
    {
        for (auto* mesh : _meshes[ filename ])
        {
            delete mesh;
        }
        _meshes.erase(filename);
    }
    else if (extension == ".shader")
    {
        delete _shaders[ filename ];
        _shaders.erase(filename);
    }
    else if (extension == ".mat")
    {
        delete _materials[ filename ];
        _materials.erase(filename);
    }
    else if (extension == ".png")
    {
        delete _textures[ filename ];
        _textures.erase(filename);
    }

    load_asset(path);
}

const std::vector<mesh*> asset_manager::meshes() const
{
    std::vector<mesh*> result;
    for (auto& [ _, value ] : _meshes)
    {
        result.insert(result.end(), value.begin(), value.end());
    }
    return result;
}

const std::vector<material*> asset_manager::materials() const
{
    std::vector<material*> result;
    for (auto& [ _, value ] : _materials)
    {
        result.push_back(value);
    }
    return result;
}

const std::vector<image*> asset_manager::textures() const
{
    std::vector<image*> result;
    for (auto& [ _, value ] : _textures)
    {
        result.push_back(value);
    }
    return result;
}

const std::vector<shader_program*> asset_manager::shaders() const
{
    std::vector<shader_program*> result;
    for (auto& [ _, value ] : _shaders)
    {
        result.push_back(value);
    }
    return result;
}

shader_program* asset_manager::get_shader(std::string_view name) const
{
    return _shaders.contains(name) ? _shaders.find(name)->second : nullptr;
}

material* asset_manager::get_material(std::string_view name) const
{
    return _materials.contains(name) ? _materials.find(name)->second : nullptr;
}

asset_manager* asset_manager::default_asset_manager()
{
    if (_instance == nullptr)
    {
        _instance = new asset_manager;
    }

    return _instance;
}

asset_manager* asset_manager::_instance = nullptr;
