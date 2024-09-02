#pragma once

#include "core/asset_loaders/asset_loader.hpp"

namespace graphics
{
class material;
}

class asset_loader_MAT : public asset_loader
{
public:
    ~asset_loader_MAT() = default;
    void load(std::string_view path) override;

    std::shared_ptr<graphics::material> get_material();

private:
    std::shared_ptr<graphics::material> _material;
};
