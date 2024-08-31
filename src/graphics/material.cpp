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

void material::declare_property(std::string_view name,
                                material_property::data_type type)
{
    material_property property;
    property._name = name;
    property._type = type;

    if (type == material_property::data_type::type_image)
    {
        property._special = _textures_count++;
    }

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

bool material::has_property(std::string_view name) const
{
    return _property_map.contains(name);
}

void material::set_property_value(std::string_view name, std::any value)
{
    if (!has_property(name))
    {
        log()->error("The material has no property \"{}\"", name);
        return;
    }

    // not using unordered_map.at to have generic string comparison
    property_map_t::iterator found_iterator = _property_map.find(name);
    found_iterator->second._value = std::move(value);
}

void material::activate() const
{
    for (const auto& [ name, property ] : _property_map)
    {
        if (!property._value.has_value())
        {
            continue;
        }

        if (property._type == material_property::data_type::type_image)
        {
            auto* t = std::any_cast<texture*>(property._value);
            t->set_active_texture(property._special);
            _shader->set_property(name, property._special);
        }
        else
        {
            _shader->set_property(name, property._value);
        }
    }
    _shader->activate();
}

void material::deactivate() const { }
