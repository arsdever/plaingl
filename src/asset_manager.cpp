#include "asset_manager.hpp"

#include "asset_loaders/fbx.hpp"
#include "asset_loaders/jpg.hpp"
#include "asset_loaders/mat.hpp"
#include "asset_loaders/png.hpp"
#include "asset_loaders/shader.hpp"
#include "file.hpp"
#include "logging.hpp"

namespace
{
static logger log() { return get_logger("asset_manager"); }
} // namespace

void asset_manager::load_asset(std::string_view path)
{
    auto [ _, filename, extension ] = parse_path(path);
#ifdef GAMIFY_SUPPORTS_FBX
    if (extension == ".fbx")
    {
        asset_loader_FBX fbx_loader;
        fbx_loader.load(path);
        auto [ it, success ] =
            _meshes.try_emplace(filename, fbx_loader.get_meshes());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_SHADER
    if (extension == ".shader")
    {
        asset_loader_SHADER shader_loader;
        shader_loader.load(path);
        auto [ it, success ] = _shader_programs.try_emplace(
            filename, shader_loader.get_shader_program());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_MAT
    if (extension == ".mat")
    {
        asset_loader_MAT mat_loader;
        mat_loader.load(path);
        auto [ it, success ] =
            _materials.try_emplace(filename, mat_loader.get_material());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_JPG
    if (extension == ".jpg" || extension == ".jpeg")
    {
        asset_loader_JPG jpg_loader;
        jpg_loader.load(path);
        auto [ it, success ] =
            _images.try_emplace(filename, jpg_loader.get_image());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_PNG
    if (extension == ".png")
    {
        asset_loader_PNG png_loader;
        png_loader.load(path);
        auto [ it, success ] =
            _images.try_emplace(filename, png_loader.get_image());
        return;
    }
#endif

    log()->error("Asset manager doesn't support {} format", extension);
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
    for (auto& [ _, value ] : _images)
    {
        result.push_back(value);
    }
    return result;
}

const std::vector<shader_program*> asset_manager::shaders() const
{
    std::vector<shader_program*> result;
    for (auto& [ _, value ] : _shader_programs)
    {
        result.push_back(value);
    }
    return result;
}

shader_program* asset_manager::get_shader(std::string_view name) const
{
    return _shader_programs.contains(name) ? _shader_programs.find(name)->second
                                           : nullptr;
}

material* asset_manager::get_material(std::string_view name) const
{
    return _materials.contains(name) ? _materials.find(name)->second : nullptr;
}

image* asset_manager::get_image(std::string_view name) const
{
    return _images.contains(name) ? _images.find(name)->second : nullptr;
}

mesh* asset_manager::get_mesh(std::string_view name) const
{
    return _meshes.contains(name) ? (_meshes.find(name)->second.empty()
                                         ? nullptr
                                         : _meshes.find(name)->second[ 0 ])
                                  : nullptr;
}

#define DEFINE_ITERATOR(type)                                                 \
    template <>                                                               \
    bool asset_manager::for_each<type>(                                       \
        std::function<bool(std::string_view, const type* const&)> func) const \
    {                                                                         \
        for (const auto& [ name, value ] : _##type##s)                        \
        {                                                                     \
            if (func(name, value))                                            \
            {                                                                 \
                continue;                                                     \
            }                                                                 \
        }                                                                     \
        return true;                                                          \
    }

DEFINE_ITERATOR(image);
DEFINE_ITERATOR(material);
DEFINE_ITERATOR(shader_program);

asset_manager* asset_manager::default_asset_manager()
{
    if (_instance == nullptr)
    {
        _instance = new asset_manager;
    }

    return _instance;
}

asset_manager* asset_manager::_instance = nullptr;
