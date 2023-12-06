#pragma once

#include <string_view>
#include <vector>

class mesh;
class material;
class image;
class shader_program;

class asset_manager
{
public:
    asset_manager() = default;

    void load_asset(std::string_view path);
    const std::vector<mesh*>& meshes() const;
    const std::vector<material*>& materials() const;
    const std::vector<image*>& textures() const;

    static asset_manager* default_asset_manager();

private:
    std::vector<mesh*> _meshes;
    std::vector<material*> _materials;
    std::vector<image*> _textures;
    std::vector<shader_program*> _shaders;
    static asset_manager* _instance;
};
