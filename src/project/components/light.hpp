#pragma once

#include <nlohmann/json_fwd.hpp>

#include "project/component.hpp"

namespace components
{
class light : public component
{
public:
    enum class type
    {
        OMNI,
        SPOTLIGHT,
        SPHERICAL,
        DIRECTIONAL
    };

public:
    light(game_object& obj);
    ~light();

    glm::dvec4 get_color() const;
    void set_color(glm::dvec4 color);

    float get_intensity() const;
    void set_intensity(float intensity);

    double get_radius() const;
    void set_radius(double radius);

    type get_type() const;
    void set_type(type light_type);

    static constexpr std::string_view type_name = "light";

    template <typename ST>
    void serialize(ST& s);
    void deserialize(const nlohmann::json& j);

private:
    glm::dvec4 _color { 1.0 };
    double _intensity { 1.0 };
    type _light_type { type::OMNI };
    double _radius { 1.0 };
};
} // namespace components
