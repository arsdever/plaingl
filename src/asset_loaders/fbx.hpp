#pragma once

#include <vector>

#include "asset_loaders/asset_loader.hpp"

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
    camera* load_camera(const aiCamera* ai_camera);
    light* load_light(const aiLight* ai_light);

private:
    mesh* _mesh;
};
