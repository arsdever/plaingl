#pragma once

#include <vector>

#include "assets/asset.hpp"
#include "vertex.hpp"

class mesh_asset : public asset
{
public:
    mesh_asset(std::string_view id, std::vector<vertex> vertices, std::vector<int> indices);

    const std::vector<vertex>& vertices() const;
    const std::vector<int>& indices() const;

private:
    std::vector<vertex> _vertices;
    std::vector<int> _indices;
};
