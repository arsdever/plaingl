#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "base.hpp"
#include "utils.hpp"

class mesh_asset;
class material;
class image;
class shader_program;

class asset_manager
{
private:
    asset_manager() = default;

public:
    void load_asset(std::string_view path);
    const std::vector<sp<mesh_asset>>& meshes() const;
    const std::vector<material*> materials() const;
    const std::vector<image*> textures() const;
    const std::vector<shader_program*> shaders() const;

    shader_program* get_shader(std::string_view name) const;
    material* get_material(std::string_view name) const;

    static asset_manager* default_asset_manager();

private:
    template <typename T>
    using asset_map =
        std::unordered_map<std::string, T, string_hash, std::equal_to<>>;

    asset_map<sp<mesh_asset>> _meshes;
    asset_map<material*> _materials;
    asset_map<image*> _textures;
    asset_map<shader_program*> _shaders;

    mutable std::pair<std::vector<sp<mesh_asset>>, bool> _cached_meshes_result {
        {}, false
    };

    static asset_manager* _instance;
};
