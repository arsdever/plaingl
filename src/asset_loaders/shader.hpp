#pragma once

#include "asset_loaders/asset_loader.hpp"

class shader_program;

class asset_loader_SHADER : public asset_loader
{
public:
    ~asset_loader_SHADER() = default;
    void load(std::string_view path) override;

    shader_program* get_shader_program();

private:
    shader_program* _shader_program = nullptr;
};
