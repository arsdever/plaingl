#pragma once

#include <unordered_map>

#include "material_property.hpp"
#include "utils.hpp"

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
    void set_shader_program(shader_program* prog);

    void declare_property(std::string_view name,
                          material_property::data_type type);

    void activate();

private:
    shader_program* _shader_program;
    std::unordered_map<std::string,
                       material_property,
                       string_hash,
                       std::equal_to<>>
        _property_map;
};
