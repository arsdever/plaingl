#include "graphics/material.hpp"

#include "common/logging.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

namespace
{
static inline logger log() { return get_logger("material"); }
} // namespace

material::material() = default;

material::material(material&& mat)
{
    _shader = mat._shader;
    _property_map = std::move(mat._property_map);
    mat._shader = 0;
}

material& material::operator=(material&& mat)
{
    _shader = mat._shader;
    _property_map = std::move(mat._property_map);
    mat._shader = 0;
    return *this;
}

material::~material() = default;

graphics::shader* material::program() const { return _shader; }

void material::set_shader_program(graphics::shader* prog) { _shader = prog; }

void material::set_property_value(std::string_view name, std::any value)
{
    _property_map[ std::string(name) ] = std::move(value);
}

void material::activate() const
{
    int texture_binding_index = 0;
    for (const auto& [ name, value ] : _property_map)
    {
        if (value.type() == typeid(texture*))
        {
            const auto* t = std::any_cast<texture*>(value);
            t->set_active_texture(texture_binding_index);
            _shader->set_property(name, texture_binding_index);
        }
        else
        {
            _shader->set_property(name, value);
        }
    }

    _shader->activate();
}

void material::deactivate() const { }

material material::from_shader(graphics::shader* s)
{
    auto mat = material();
    mat._shader = s;
    return mat;
}
