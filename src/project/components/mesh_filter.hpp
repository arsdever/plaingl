#pragma once

#include <nlohmann/json_fwd.hpp>

#include "graphics/graphics_fwd.hpp"
#include "project/component.hpp"

class game_object;

namespace components
{
class mesh_filter : public component
{
public:
    mesh_filter(game_object& obj);

    static constexpr std::string_view type_name = "mesh_filter";

    mesh* get_mesh() const;
    void set_mesh(mesh* m);

    template <typename ST>
    void serialize(ST& s);
    void deserialize(const nlohmann::json& j);

private:
    mesh* _mesh;
};
} // namespace components
