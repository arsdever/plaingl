#include "asset_manager.hpp"

#include "asset_loaders/fbx.hpp"
#include "asset_loaders/jpg.hpp"
#include "asset_loaders/mat.hpp"
#include "asset_loaders/png.hpp"
#include "asset_loaders/shader.hpp"
#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "directory.hpp"
#include "graphics/image.hpp"
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"

namespace core
{
struct asset_manager::impl
{
    std::string project_path;
    common::file_watcher directory_watcher;

    template <typename T>
    using asset_map =
        std::unordered_map<std::string, T, string_hash, std::equal_to<>>;

    asset_map<mesh*> _meshs;
    asset_map<std::shared_ptr<graphics::material>> _materials;
    asset_map<image*> _images;
    asset_map<std::shared_ptr<graphics::shader>> _shaders;
};

namespace
{
static logger log() { return get_logger("asset_manager"); }
} // namespace

void asset_manager::initialize(asset_manager* existing_instance)
{
    if (_impl)
        return;

    _impl = existing_instance->_impl;
}

void asset_manager::initialize(std::string_view resource_path)
{
    _impl = std::make_shared<impl>();
    _impl->project_path = common::filesystem::path(resource_path).full_path();

    scan_directory();
    setup_directory_watch();
    initialize_quad_mesh();
    initialize_surface_shader();
}

void asset_manager::shutdown()
{
    if (!_impl)
        return;

    _impl = nullptr;
}

void asset_manager::load_asset(std::string_view path)
{
    auto filepath = common::filesystem::path(path);
    std::string name_string = std::string(filepath.stem());
#ifdef GAMIFY_SUPPORTS_FBX
    if (filepath.extension() == ".fbx")
    {
        asset_loader_FBX fbx_loader;
        fbx_loader.load(path);
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_SHADER
    if (filepath.extension() == ".shader")
    {
        asset_loader_SHADER shader_loader;
        shader_loader.load(path);
        shader_loader.get_shader()->set_name(name_string);
        auto [ it, success ] = _impl->_shaders.try_emplace(
            name_string, shader_loader.get_shader());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_MAT
    if (filepath.extension() == ".mat")
    {
        asset_loader_MAT mat_loader;
        mat_loader.load(path);
        auto [ it, success ] = _impl->_materials.try_emplace(
            name_string, mat_loader.get_material());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_JPG
    if (filepath.extension() == ".jpg" || filepath.extension() == ".jpeg")
    {
        asset_loader_JPG jpg_loader;
        jpg_loader.load(path);
        auto [ it, success ] =
            _impl->_images.try_emplace(name_string, jpg_loader.get_image());
        return;
    }
#endif
#ifdef GAMIFY_SUPPORTS_PNG
    if (filepath.extension() == ".png")
    {
        asset_loader_PNG png_loader;
        png_loader.load(path);
        auto [ it, success ] =
            _impl->_images.try_emplace(name_string, png_loader.get_image());
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
    if (filepath.extension() == ".png")
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
    _impl->_meshs.emplace(std::string(name), asset);
}

const std::vector<mesh*> asset_manager::meshes()
{
    std::vector<mesh*> result;
    for (auto& [ _, value ] : _impl->_meshs)
    {
        result.push_back(value);
    }
    return result;
}

const std::vector<std::shared_ptr<graphics::material>>
asset_manager::materials()
{
    std::vector<std::shared_ptr<graphics::material>> result;
    for (auto& [ _, value ] : _impl->_materials)
    {
        result.push_back(value);
    }
    return result;
}

const std::vector<image*> asset_manager::textures()
{
    std::vector<image*> result;
    for (auto& [ _, value ] : _impl->_images)
    {
        result.push_back(value);
    }
    return result;
}

const std::vector<std::shared_ptr<graphics::shader>> asset_manager::shaders()
{
    std::vector<std::shared_ptr<graphics::shader>> result;
    for (auto& [ _, value ] : _impl->_shaders)
    {
        result.push_back(value);
    }
    return result;
}

std::shared_ptr<graphics::shader>
asset_manager::get_shader(std::string_view name)
{
    return _impl->_shaders.contains(name) ? _impl->_shaders.find(name)->second
                                          : nullptr;
}

std::shared_ptr<graphics::material>
asset_manager::get_material(std::string_view name)
{
    return _impl->_materials.contains(name)
               ? _impl->_materials.find(name)->second
               : nullptr;
}

image* asset_manager::get_image(std::string_view name)
{
    return _impl->_images.contains(name) ? _impl->_images.find(name)->second
                                         : nullptr;
}

mesh* asset_manager::get_mesh(std::string_view name)
{
    return _impl->_meshs.contains(name) ? _impl->_meshs.find(name)->second
                                        : nullptr;
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
    _impl->_meshs.try_emplace("quad", quad_mesh);
}

void asset_manager::initialize_surface_shader()
{
    load_asset("resources/standard/surface.shader");
    load_asset("resources/standard/standard.mat");
}

std::string_view asset_manager::internal_resource_path() { return ""; }

void asset_manager::scan_directory()
{
    auto project_path = common::filesystem::path(_impl->project_path);
    std::function<void(common::filesystem::path)> scan_dir;
    scan_dir = [ & ](common::filesystem::path path)
    {
        common::directory dir(std::string(path.full_path()));
        dir.visit_files(
            [ scan_dir, path ](std::string file_path, bool is_dir)
        {
            if (is_dir)
            {
                if (file_path == "." || file_path == "..")
                {
                    return;
                }

                scan_dir(path / file_path);
            }
            else
            {
                auto asset_path = path / file_path;
                load_asset(asset_path.full_path());
            }
        });
    };
    scan_dir(project_path);
}

void asset_manager::setup_directory_watch()
{
    _impl->directory_watcher = common::file_watcher(
        _impl->project_path,
        [](std::string_view path, common::file_change_type change)
    {
        switch (change)
        {
        case common::file_change_type::created: load_asset(path); break;
        // case common::file_change_type::modified: reload_asset(path); break;
        // case common::file_change_type::removed: unload_asset(path); break;
        default: break;
        }
    });
}

std::shared_ptr<asset_manager::impl> asset_manager::_impl = nullptr;
} // namespace core
