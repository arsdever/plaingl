#include <glad/gl.h>
#include <glm/ext.hpp>

#include "material.hpp"

#include "shader.hpp"

material::material() = default;

material::material(material&& mat)
{
    _shader_program = mat._shader_program;
    mat._shader_program = 0;
    _properties = std::move(mat._properties);
    _name_property_map = std::move(mat._name_property_map);
}

material& material::operator=(material&& mat)
{
    _shader_program = mat._shader_program;
    mat._shader_program = 0;
    _properties = std::move(mat._properties);
    _name_property_map = std::move(mat._name_property_map);
    return *this;
}

material::~material() = default;

shader_program* material::program() const { return _shader_program; }

const std::vector<uniform_info>& material::properties() const
{
    return _properties;
}

void material::set_shader_program(shader_program* prog)
{
    _shader_program = prog;
}

void material::activate()
{
    for (auto& property : _properties)
    {
        _shader_program->set_uniform(property._index, property._value);
    }
}
