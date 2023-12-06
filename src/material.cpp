#include <glad/gl.h>
#include <glm/ext.hpp>

#include "material.hpp"

#include "logging.hpp"
#include "shader.hpp"

namespace
{
static inline logger log() { return get_logger("material"); }
} // namespace

material::material() = default;

material::material(material&& mat)
{
    _shader_program = mat._shader_program;
    mat._shader_program = 0;
}

material& material::operator=(material&& mat)
{
    _shader_program = mat._shader_program;
    mat._shader_program = 0;
    return *this;
}

material::~material() = default;

shader_program* material::program() const { return _shader_program; }

void material::set_shader_program(shader_program* prog)
{
    _shader_program = prog;
}

void material::declare_property(std::string_view name,
                                material_property::data_type type)
{
    material_property property;
    property._name = name;
    property._type = type;
    if (auto [ existing, success ] =
            _property_map.try_emplace(std::string(name), std::move(property));
        !success)
    {
        std::string_view reason = "Unknown";
        if (existing != _property_map.end())
        {
            reason = "Another property with the same name already exists";
        }
        log()->error("Failed to add property \"{}\": {}", name, reason);
    }
}

void material::activate() { }
