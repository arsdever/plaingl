#pragma once

#include <vector>

#include "asset_loaders/asset_loader.hpp"

class mesh;

class asset_loader_FBX : public asset_loader
{
public:
    ~asset_loader_FBX() = default;
    void load(std::string_view path) override;

    mesh* get_mesh();

private:
    mesh* _mesh;
};
