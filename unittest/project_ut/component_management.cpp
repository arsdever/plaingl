#include <gtest/gtest.h>

#include "project/components/mesh_filter.hpp"
#include "project/components/mesh_renderer.hpp"
#include "project/game_object.hpp"
#include "project/project_manager.hpp"

TEST(ProjectComponentManagement, default_transform_creation)
{
    project_manager::initialize();
    auto obj = game_object::create();
    auto tr = obj->try_get("transform");
    EXPECT_NE(tr, nullptr);
}

TEST(ProjectComponentManagement, add_component_same_as_get_component)
{
    project_manager::initialize();
    // Ensures that component returned by add command is the same as later get
    // command

    auto obj = game_object::create();
    auto& mf = obj->add("mesh_filter");
    auto mfptr = obj->try_get("mesh_filter");

    EXPECT_EQ(mf.shared_from_this(), mfptr);
}

TEST(ProjectComponentManagement, add_component_same_as_get_component_2)
{
    project_manager::initialize();
    // Ensures that component returned by add command is the same as later get
    // command
    // Checks are done through changing parameter in the component

    auto obj = game_object::create();
    obj->add<components::mesh_filter>().set_mesh(
        reinterpret_cast<mesh*>(0xff00ff00ff00ff00));
    auto& mf = obj->get("mesh_filter");

    EXPECT_EQ(static_cast<components::mesh_filter&>(mf).get_mesh(),
              reinterpret_cast<mesh*>(0xff00ff00ff00ff00));
}
