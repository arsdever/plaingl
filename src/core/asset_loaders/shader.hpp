#pragma once

#include "core/asset_loaders/asset_loader.hpp"

namespace graphics
{
class shader;
}

class asset_loader_SHADER : public asset_loader
{
public:
    ~asset_loader_SHADER() = default;
    void load(std::string_view path) override;

    graphics::shader* get_shader();

private:
    graphics::shader* _shader = nullptr;
};
