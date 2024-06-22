#pragma once

#include <nlohmann/json_fwd.hpp>

#include "project/components/component.hpp"

class game_object;
class mesh;

namespace components
{
class mesh_filter : public component
{
public:
    mesh_filter(game_object& obj);

    static constexpr const char type_name[] = "mesh_filter";
    static size_t register_component();

    mesh* get_mesh() const;
    void set_mesh(mesh* m);

    template <typename ST>
    void serialize(ST& s);
    void deserialize(const nlohmann::json& j);

private:
    mesh* _mesh;
};
} // namespace components
