#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

#include "color.hpp"
#include "material_property.hpp"

class shader_program;

class material
{
public:
    using property_t = std::variant<double, float, int, unsigned, bool, color>;

public:
    material();
    ~material();

    shader_program* program() const;
    const std::vector<material_property_info>& properties() const;

    void set_shader_program(shader_program* prog);

    template <typename T>
    void set_property(std::string_view name, T value)
    {
        if (!property_type_matches<T>(_name_property_map.at(name)._type))
        {
            return;
        }

        _shader_program->use();
        (*material_property_type_traits<T>::gl_setter)(
            _name_property_map.at(name)._index, value);
        _shader_program->unuse();
    }

private:
    template <typename T>
    bool property_type_matches(material_property_info::type type)
    {
        return type == material_property_type_traits<T>::gl_type;
    }

    void resolve_uniforms();

private:
    shader_program* _shader_program;
    std::vector<material_property_info> _properties;
    std::unordered_map<std::string_view, material_property_info&>
        _name_property_map;
};
