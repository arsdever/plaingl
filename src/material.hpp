#pragma once

#include <tuple>
#include <unordered_map>

#include "material_property.hpp"

class shader_program;

class material
{
public:
    material();
    ~material();

    shader_program* program() const;
    const std::vector<material_property_info>& properties() const;

    void set_shader_program(shader_program* prog);

    template <typename... T>
    void set_property(std::string_view name, T... value)
    {
        if (!_name_property_map.contains(name))
        {
            return;
        }

        _name_property_map.at(name)._value = std::tuple<T...>(value...);
    }

    void activate();

private:
    void resolve_uniforms();

private:
    shader_program* _shader_program;
    std::vector<material_property_info> _properties;
    std::unordered_map<std::string_view, material_property_info&>
        _name_property_map;
};
