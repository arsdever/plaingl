#pragma once

#include "common/utils.hpp"
#include "material_property.hpp"

namespace graphics
{
class shader;
}

class material
{
private:
    using property_map_t = std::unordered_map<std::string,
                                              material_property,
                                              string_hash,
                                              std::equal_to<>>;

public:
    material();
    material(material&& mat);
    material(const material& mat) = delete;
    material& operator=(material&& mat);
    material& operator=(const material& mat) = delete;
    ~material();

    graphics::shader* program() const;
    void set_shader_program(graphics::shader* prog);

    void declare_property(std::string_view name,
                          material_property::data_type type);

    bool has_property(std::string_view name) const;

    template <typename... T>
    void set_property_value(std::string_view name, T... args)
    {
        set_property_value(name, std::any { std::make_tuple(args...) });
    }

    template <typename T>
    void set_property_value(std::string_view name, T element)
    {
        set_property_value(name, std::any { element });
    }

    void activate() const;
    void deactivate() const;

private:
    void set_property_value(std::string_view name, std::any value);

private:
    graphics::shader* _shader;
    property_map_t _property_map;
    unsigned _textures_count = 0;
};
