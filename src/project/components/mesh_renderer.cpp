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
    : component(type_name, obj)
{
}

std::shared_ptr<graphics::material> mesh_renderer::get_material() const
{
    return _material;
}

void mesh_renderer::set_material(std::shared_ptr<graphics::material> m)
{
    _material = m;
}

template <>
void mesh_renderer::serialize<json_serializer>(json_serializer& s)
{
    s.add_component(nlohmann::json {
        { "type", type_id<mesh_renderer>() },
        { "is_enabled", is_enabled() },
    });
}

void mesh_renderer::deserialize(const nlohmann::json& j)
{
    set_enabled(j[ "is_enabled" ]);
}
} // namespace components
