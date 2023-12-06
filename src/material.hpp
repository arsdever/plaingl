#pragma once

#include <tuple>
#include <unordered_map>

#include "uniform_info.hpp"

class shader_program;

class material
{
public:
    material();
    material(material&& mat);
    material(const material& mat) = delete;
    material& operator=(material&& mat);
    material& operator=(const material& mat) = delete;
    ~material();

    shader_program* program() const;
    const std::vector<uniform_info>& properties() const;

    void set_shader_program(shader_program* prog);

    void activate();

private:
    shader_program* _shader_program;
    std::vector<uniform_info> _properties;
    std::unordered_map<std::string, uniform_info&> _name_property_map;
};
