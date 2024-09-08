#pragma once

#include <nlohmann/json_fwd.hpp>

#include "graphics/graphics_fwd.hpp"
#include "project/project_fwd.hpp"

#include "project/component.hpp"

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
