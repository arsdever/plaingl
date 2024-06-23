#include <entt/entt.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "project/components/mesh_renderer.hpp"

#include "project/memory_manager.hpp"
#include "project/serialization_utilities.hpp"
#include "project/serializer.hpp"
#include "project/serializer_json.hpp"

using namespace serialization::utilities;

namespace components
{
mesh_renderer::mesh_renderer(game_object& obj)
    : component("mesh_renderer", obj)
{
}

material* mesh_renderer::get_material() const { return _material; }

void mesh_renderer::set_material(material* m) { _material = m; }

template <>
void mesh_renderer::serialize<json_serializer>(json_serializer& s)
{
    s.add_component(nlohmann::json {
        { "type", entt::hashed_string("mesh_filter").value() },
        { "is_active", is_active() },
    });
}

void mesh_renderer::deserialize(const nlohmann::json& j)
{
    set_active(j[ "is_active" ]);
}
} // namespace components
