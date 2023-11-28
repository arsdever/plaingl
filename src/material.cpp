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
    shader_program::unuse();
}

void material::activate()
{
    _shader_program->use();
    for (auto& property : _properties)
    {
        if (!property._value.has_value())
        {
            continue;
        }

        int id = property._index;
        switch (property._type)
        {
        case material_property_info::type::property_type_1f:
        {
            auto [ f1 ] = std::any_cast<std::tuple<float>>(property._value);
            glUniform1f(id, f1);
            break;
        }
        case material_property_info::type::property_type_2f:
        {
            auto [ f1, f2 ] =
                std::any_cast<std::tuple<float, float>>(property._value);
            glUniform2f(id, f1, f2);
            break;
        }
        case material_property_info::type::property_type_3f:
        {
            auto [ f1, f2, f3 ] =
                std::any_cast<std::tuple<float, float, float>>(property._value);
            glUniform3f(id, f1, f2, f3);
            break;
        }
        case material_property_info::type::property_type_4f:
        {
            auto [ f1, f2, f3, f4 ] =
                std::any_cast<std::tuple<float, float, float, float>>(
                    property._value);
            glUniform4f(id, f1, f2, f3, f4);
            break;
        }
        case material_property_info::type::property_type_1i:
        {
            auto [ i1 ] = std::any_cast<std::tuple<int>>(property._value);
            glUniform1i(id, i1);
            break;
        }
        case material_property_info::type::property_type_2i:
        {
            auto [ i1, i2 ] =
                std::any_cast<std::tuple<int, int>>(property._value);
            glUniform2i(id, i1, i2);
            break;
        }
        case material_property_info::type::property_type_3i:
        {
            auto [ i1, i2, i3 ] =
                std::any_cast<std::tuple<int, int, int>>(property._value);
            glUniform3i(id, i1, i2, i3);
            break;
        }
        case material_property_info::type::property_type_4i:
        {
            auto [ i1, i2, i3, i4 ] =
                std::any_cast<std::tuple<int, int, int, int>>(property._value);
            glUniform4i(id, i1, i2, i3, i4);
            break;
        }
        case material_property_info::type::property_type_1ui:
        {
            auto [ ui1 ] = std::any_cast<std::tuple<unsigned>>(property._value);
            glUniform1ui(id, ui1);
            break;
        }
        case material_property_info::type::property_type_2ui:
        {
            auto [ ui1, ui2 ] =
                std::any_cast<std::tuple<unsigned, unsigned>>(property._value);
            glUniform2ui(id, ui1, ui2);
            break;
        }
        case material_property_info::type::property_type_3ui:
        {
            auto [ ui1, ui2, ui3 ] =
                std::any_cast<std::tuple<unsigned, unsigned, unsigned>>(
                    property._value);
            glUniform3ui(id, ui1, ui2, ui3);
            break;
        }
        case material_property_info::type::property_type_4ui:
        {
            auto [ ui1, ui2, ui3, ui4 ] = std::any_cast<
                std::tuple<unsigned, unsigned, unsigned, unsigned>>(
                property._value);
            glUniform4ui(id, ui1, ui2, ui3, ui4);
            break;
        }
        default: break;
        }
    }
}
