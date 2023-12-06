#include "assets/mesh_asset.hpp"

mesh_asset::mesh_asset(std::string_view id,
                       std::vector<vertex> vertices,
                       std::vector<int> indices)
    : asset(id)
    , _vertices(std::move(vertices))
    , _indices(std::move(indices))
{
}

const std::vector<vertex>& mesh_asset::vertices() const { return _vertices; }

const std::vector<int>& mesh_asset::indices() const { return _indices; }
