#pragma once

#include <unordered_map>

#include "material_property.hpp"
#include "utils.hpp"

class shader_program;

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

    shader_program* program() const;
    void set_shader_program(shader_program* prog);

    void declare_property(std::string_view name,
                          material_property::data_type type);

    bool has_property(std::string_view name) const;

    void set_property_value(std::string_view name, std::any value);

    void activate();

private:
    shader_program* _shader_program;
    property_map_t _property_map;
};
