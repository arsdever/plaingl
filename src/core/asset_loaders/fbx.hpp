#pragma once

#include <vector>

#include "core/asset_loaders/asset_loader.hpp"

class mesh;
class camera;
class light;

class aiMesh;
class aiCamera;
class aiLight;

class asset_loader_FBX : public asset_loader
{
public:
    ~asset_loader_FBX() = default;
    void load(std::string_view path) override;

private:
    mesh* load_mesh(std::vector<const aiMesh*> ai_submeshes);

private:
    mesh* _mesh;
};
