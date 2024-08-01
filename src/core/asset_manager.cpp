#include "asset_manager.hpp"

#include "asset_loaders/fbx.hpp"
#include "asset_loaders/jpg.hpp"
#include "asset_loaders/mat.hpp"
#include "asset_loaders/png.hpp"
#include "asset_loaders/shader.hpp"
#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "graphics/image.hpp"
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"

namespace
{
static logger log() { return get_logger("asset_manager"); }
} // namespace

void asset_manager::load_asset(std::string_view path)
{
    auto filepath = common::filesystem::path(path);
    std::string name_string = std::string(filepath.stem());
#ifdef GAMIFY_SUPPORTS_FBX
    if (filepath.extension() == "fbx")
    {
        asset_loader_FBX fbx_loader;
        fbx_loader.load(path);
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_SHADER
    if (filepath.extension() == "shader")
    {
        asset_loader_SHADER shader_loader;
        shader_loader.load(path);
        shader_loader.get_shader_program()->set_name(name_string);
        auto [ it, success ] = _shader_programs.try_emplace(
            name_string, shader_loader.get_shader_program());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_MAT
    if (filepath.extension() == "mat")
    {
        asset_loader_MAT mat_loader;
        mat_loader.load(path);
        auto [ it, success ] =
            _materials.try_emplace(name_string, mat_loader.get_material());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_JPG
    if (filepath.extension() == "jpg" || filepath.extension() == "jpeg")
    {
        asset_loader_JPG jpg_loader;
        jpg_loader.load(path);
        auto [ it, success ] =
            _images.try_emplace(name_string, jpg_loader.get_image());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_PNG
    if (filepath.extension() == "png")
    {
        asset_loader_PNG png_loader;
        png_loader.load(path);
        auto [ it, success ] =
            _images.try_emplace(name_string, png_loader.get_image());
        return;
    }
#endif

    log()->error("Asset manager doesn't support {} format",
                 filepath.extension());
}

template <>
void asset_manager::save_asset<image>(std::string_view path, const image* img)
{
    auto filepath = common::filesystem::path(path);
#ifdef GAMIFY_SUPPORTS_PNG
    if (filepath.extension() == "png")
    {
        asset_loader_PNG png_loader;
        png_loader.set_image(const_cast<image*>(img));
        png_loader.save(path);
        return;
    }
#endif
}

template <>
void asset_manager::register_asset<mesh>(std::string_view name, mesh* asset)
{
    _meshs.emplace(std::string(name), asset);
}

const std::vector<mesh*> asset_manager::meshes() const
{
    std::vector<mesh*> result;
    for (auto& [ _, value ] : _meshs)
    {
        result.push_back(value);
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
    return _meshs.contains(name) ? _meshs.find(name)->second : nullptr;
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

DEFINE_ITERATOR(mesh);
DEFINE_ITERATOR(image);
DEFINE_ITERATOR(material);
DEFINE_ITERATOR(shader_program);

asset_manager* asset_manager::default_asset_manager()
{
    if (_instance == nullptr)
    {
        initialize();
    }

    return _instance;
}

void asset_manager::initialize()
{
    _instance = new asset_manager;
    initialize_quad_mesh();
    initialize_surface_shader();
}

void asset_manager::initialize_quad_mesh()
{
    auto quad_mesh = new mesh();
    std::vector<vertex3d> vertices;
    vertices.resize(4);
    vertices[ 0 ].position() = { -1.0f, -1.0f, 0.0f };
    vertices[ 1 ].position() = { 1.0f, -1.0f, 0.0f };
    vertices[ 2 ].position() = { 1.0f, 1.0f, 0.0f };
    vertices[ 3 ].position() = { -1.0f, 1.0f, 0.0f };

    vertices[ 0 ].uv() = { 0.0f, 0.0f };
    vertices[ 1 ].uv() = { 1.0f, 0.0f };
    vertices[ 2 ].uv() = { 1.0f, 1.0f };
    vertices[ 3 ].uv() = { 0.0f, 1.0f };

    quad_mesh->set_vertices(std::move(vertices));
    quad_mesh->set_indices({ 0, 1, 2, 0, 2, 3 });
    quad_mesh->init();
    _instance->_meshs.try_emplace("quad", quad_mesh);
}

void asset_manager::initialize_surface_shader()
{
    _instance->load_asset("resources/standard/surface.shader");
    _instance->load_asset("resources/standard/standard.mat");
}

std::string_view asset_manager::internal_resource_path() { return ""; }

asset_manager* asset_manager::_instance = nullptr;
