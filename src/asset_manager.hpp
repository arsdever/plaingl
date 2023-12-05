#pragma once

#include <string_view>
#include <vector>

class mesh;
class image;

class asset_manager
{
public:
    asset_manager() = default;

    void load_asset(std::string_view path);
    const std::vector<mesh*>& meshes() const;
    const std::vector<image*>& textures() const;

private:
    std::vector<mesh*> _meshes;
    std::vector<image*> _textures;
};
