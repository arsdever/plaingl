#include <glad/gl.h>

#include "material.hpp"

#include "shader.hpp"

material::material() = default;

material::~material() = default;

shader_program* material::program() const { return _shader_program; }

const std::vector<material_property_info>& material::properties() const
{
    return _properties;
}

void material::set_shader_program(shader_program* prog)
{
    _shader_program = prog;
    resolve_uniforms();
}

void material::resolve_uniforms()
{
    _shader_program->use();
    _properties.clear();
    _name_property_map.clear();
    int uniform_count = 0;
    glGetProgramiv(_shader_program->id(), GL_ACTIVE_UNIFORMS, &uniform_count);
    if (uniform_count == 0)
    {
        return;
    }

    std::string buffer;
    int length;
    int size;
    unsigned type;
    buffer.resize(512);
    _properties.resize(uniform_count);
    for (int i = 0; i < uniform_count; ++i)
    {
        glGetActiveUniform(_shader_program->id(),
                           i,
                           512,
                           &length,
                           &size,
                           &type,
                           buffer.data());
        _properties[ i ]._name = buffer;
        _properties[ i ]._name.resize(length);
        _properties[ i ]._index = i;
        _properties[ i ]._size = size;
        _properties[ i ]._type = material_property_info::type(type);
        // TODO: verify emplace did add element
        _name_property_map.try_emplace(_properties[ i ]._name,
                                       _properties[ i ]);
    }
    _shader_program->unuse();
}
