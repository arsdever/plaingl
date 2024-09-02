#pragma once

#include "common/utils.hpp"

class mesh;
class image;
namespace graphics
{
class material;
class shader;
} // namespace graphics

namespace core
{
class asset_manager
{
private:
    asset_manager() = default;

public:
    static void initialize(asset_manager* existing_instance);
    static void initialize(std::string_view resource_path);
    static void shutdown();
    static void load_asset(std::string_view path);
    template <typename T>
    static void save_asset(std::string_view path, const T* asset);

    template <typename T>
    static void register_asset(std::string_view name, T* asset);

    static const std::vector<mesh*> meshes();
    static const std::vector<std::shared_ptr<graphics::material>> materials();
    static const std::vector<image*> textures();
    static const std::vector<std::shared_ptr<graphics::shader>> shaders();

    static mesh* get_mesh(std::string_view name);
    static std::shared_ptr<graphics::shader> get_shader(std::string_view name);
    static std::shared_ptr<graphics::material>
    get_material(std::string_view name);
    static image* get_image(std::string_view name);

    static asset_manager* default_asset_manager();

private:
    static void initialize_quad_mesh();
    static void initialize_surface_shader();
    static std::string_view internal_resource_path();

    static void scan_directory();
    static void setup_directory_watch();

private:
    struct impl;
    static std::shared_ptr<impl> _impl;
};
} // namespace core
