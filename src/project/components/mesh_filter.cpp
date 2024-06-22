#include <entt/entt.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "project/components/mesh_filter.hpp"

#include "project/memory_manager.hpp"
#include "project/serialization_utilities.hpp"
#include "project/serializer.hpp"
#include "project/serializer_json.hpp"

using namespace serialization::utilities;

namespace components
{
mesh_filter::mesh_filter(game_object& obj)
    : component("mesh_filter", obj)
{
}

mesh* mesh_filter::get_mesh() const { return _mesh; }

void mesh_filter::set_mesh(mesh* m) { _mesh = m; }

template <>
void mesh_filter::serialize<json_serializer>(json_serializer& s)
{
    s.add_component(nlohmann::json {
        { "type", entt::hashed_string("mesh_filter").value() },
        { "is_active", is_active() },
    });
}

void mesh_filter::deserialize(const nlohmann::json& j)
{
    set_active(j[ "is_active" ]);
}

size_t mesh_filter::register_component()
{
    auto id = entt::hashed_string(type_name);
    entt::meta<mesh_filter>()
        .type(id)
        .base<component>()
        .ctor<game_object&>()
        .func<&mesh_filter::serialize<json_serializer>>(
            entt::hashed_string("serialize"))
        .func<&mesh_filter::serialize<json_serializer>>(
            entt::hashed_string("deserialize"));
    memory_manager::register_component_type<mesh_filter>();
    return id;
}
} // namespace components
