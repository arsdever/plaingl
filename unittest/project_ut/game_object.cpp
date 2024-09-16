#include <gtest/gtest.h>

#include "project/game_object.hpp"

#include "graphics/mesh.hpp"
#include "project/components/mesh_filter.hpp"
#include "project/components/transform.hpp"
#include "project/project_manager.hpp"

TEST(ProjectGameObject, add_and_get_componennt)
{
    project_manager::initialize();

    auto obj = game_object::create();
    obj->set_name("test");

    auto tr = obj->try_get<components::transform>();
    EXPECT_EQ(tr, obj->get_transform().shared_from_this());

    auto& mf = obj->add<components::mesh_filter>();

    EXPECT_EQ(obj->try_get<components::mesh_filter>(), mf.shared_from_this());
    mesh* m = (mesh*)(5000);
    mf.set_mesh(m);

    EXPECT_EQ(obj->get<components::mesh_filter>().get_mesh(), m);
    auto& mesh_filter =
        static_cast<components::mesh_filter&>(obj->get("mesh_filter"));
    EXPECT_EQ(mesh_filter.get_mesh(), m);
}
