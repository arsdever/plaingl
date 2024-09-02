#pragma once

#include <nlohmann/json_fwd.hpp>

#include "project/component.hpp"

class game_object;
namespace graphics
{
class material;
}

namespace components
{
class mesh_renderer : public component
{
public:
    mesh_renderer(game_object& obj);

    static constexpr std::string_view type_name = "mesh_renderer";

    std::shared_ptr<graphics::material> get_material() const;
    void set_material(std::shared_ptr<graphics::material> m);

    template <typename ST>
    void serialize(ST& s);
    void deserialize(const nlohmann::json& j);

private:
    std::shared_ptr<graphics::material> _material;
};
} // namespace components
