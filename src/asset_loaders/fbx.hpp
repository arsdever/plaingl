#pragma once

#include <memory>
#include <vector>

#include "asset_loaders/asset_loader.hpp"

class mesh_asset;

class asset_loader_FBX : public asset_loader
{
public:
    ~asset_loader_FBX() = default;
    void load(std::string_view path) override;

    const std::vector<std::shared_ptr<mesh_asset>>& get_meshes() const;

private:
    std::vector<std::shared_ptr<mesh_asset>> _meshes;
};
