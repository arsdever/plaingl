#include <nlohmann/json.hpp>

#include "project/components/light.hpp"

#include "common/logging.hpp"
#include "project/serializer_json.hpp"

namespace
{
static logger log() { return get_logger("light"); }
} // namespace

namespace components
{
light::light(game_object& obj)
    : component(type_name, obj)
{
}

light::~light() = default;

glm::dvec4 light::get_color() const { return _color; }

void light::set_color(glm::dvec4 color) { _color = color; }

float light::get_intensity() const { return _intensity; }

void light::set_intensity(float intensity) { _intensity = intensity; }

double light::get_radius() const { return _radius; }

void light::set_radius(double radius) { _radius = radius; }

light::type light::get_type() const { return _light_type; }

void light::set_type(type light_type) { _light_type = light_type; }

template <>
void light::serialize(json_serializer& j)
{
    j.add_component(nlohmann::json {
        { "type", light::type_name },
        { "is_enabled", is_enabled() },
    });
}

void light::deserialize(const nlohmann::json& j)
{
    set_enabled(j[ "is_enabled" ]);
}
} // namespace components
