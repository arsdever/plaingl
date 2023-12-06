#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "utils.hpp"

class mesh;
class material;
class image;
class shader_program;

class asset_manager
{
private:
    asset_manager() = default;

public:
    void load_asset(std::string_view path);
    const std::vector<mesh*> meshes() const;
    const std::vector<material*> materials() const;
    const std::vector<image*> textures() const;
    const std::vector<shader_program*> shaders() const;

    static asset_manager* default_asset_manager();

private:
    template <typename T>
    using asset_map =
        std::unordered_map<std::string, T, string_hash, std::equal_to<>>;

    asset_map<std::vector<mesh*>> _meshes;
    asset_map<material*> _materials;
    asset_map<image*> _textures;
    asset_map<shader_program*> _shaders;
    static asset_manager* _instance;
};
