#include "graphics/material.hpp"

#include "asset_management/asset_manager.hpp"
#include "common/logging.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

namespace graphics
{
namespace
{
static inline logger log() { return get_logger("material"); }
} // namespace

material::material() = default;

material::material(material&& mat)
{
    _shader = mat._shader;
    _property_map = std::move(mat._property_map);
    mat._shader = {};
}

material& material::operator=(material&& mat)
{
    _shader = mat._shader;
    _property_map = std::move(mat._property_map);
    mat._shader = {};
    return *this;
}

material::~material() = default;

std::shared_ptr<graphics::shader> material::program() const
{
    return _shader.lock();
}

void material::set_shader_program(std::shared_ptr<graphics::shader> prog)
{
    _shader = prog;
}

void material::set_property_value(std::string_view name, std::any value)
{
    _property_map[ std::string(name) ] = std::move(value);
}

void material::activate() const
{
    auto s = program();
    if (!s)
    {
        if (_fallback_shader)
        {
            _fallback_shader->activate();
        }
        return;
    }

    int texture_binding_index = 0;
    for (const auto& [ name, value ] : _property_map)
    {
        if (value.type() == typeid(std::shared_ptr<graphics::texture>))
        {
            auto t = std::any_cast<std::shared_ptr<graphics::texture>>(value);
            t->set_active_texture(texture_binding_index);
            s->set_property(name, texture_binding_index++);
        }
        else if (value.type() == typeid(texture*))
        {
            const auto* t = std::any_cast<texture*>(value);
            t->set_active_texture(texture_binding_index);
            s->set_property(name, texture_binding_index++);
        }
        else
        {
            s->set_property(name, value);
        }
    }

    s->activate();
}

void material::deactivate() const { }

void material::set_fallback_shader(std::shared_ptr<graphics::shader> shader)
{
    _fallback_shader = std::move(shader);
}

std::shared_ptr<graphics::shader> material::_fallback_shader = {};
} // namespace graphics
