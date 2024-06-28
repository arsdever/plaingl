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
    : component(type_name, obj)
{
}

mesh* mesh_filter::get_mesh() const { return _mesh; }

void mesh_filter::set_mesh(mesh* m) { _mesh = m; }

template <>
void mesh_filter::serialize<json_serializer>(json_serializer& s)
{
    s.add_component(nlohmann::json {
        { "type", type_id<mesh_filter>() },
        { "is_enabled", is_enabled() },
    });
}

void mesh_filter::deserialize(const nlohmann::json& j)
{
    set_enabled(j[ "is_enabled" ]);
}
} // namespace components
