#pragma once

#include "core/asset_loaders/asset_loader.hpp"

class material;

class asset_loader_MAT : public asset_loader
{
public:
    ~asset_loader_MAT() = default;
    void load(std::string_view path) override;

    material* get_material();

private:
    material* _material;
};
