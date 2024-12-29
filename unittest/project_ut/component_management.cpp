#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "project/component_interface/component_registry.hpp"
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
    std::shared_ptr<graphics::mesh> m = std::shared_ptr<graphics::mesh>(
        reinterpret_cast<graphics::mesh*>(0xff00ff00ff00ff00), [](auto) { });
    obj->add<components::mesh_filter>().set_mesh(m);
    auto& mf = obj->get<components::mesh_filter>();

    EXPECT_EQ(mf.get_mesh().get(),
              reinterpret_cast<graphics::mesh*>(0xff00ff00ff00ff00));
}

class test_component : public component
{
public:
    using base = component;

    test_component(game_object& obj)
        : component(type_name, obj)
    {
    }

    MOCK_METHOD(void, on_init, (), (override));
    MOCK_METHOD(void, on_update, (), (override));
    MOCK_METHOD(void, on_deinit, (), (override));

    static constexpr std::string_view type_name = "test_component";
};

TEST(ProjectComponentManagement, nested_object_component_update)
{
    project_manager::initialize();
    component_registry::register_component<test_component>("test_component");

    auto obj = game_object::create();

    auto nested = game_object::create();
    obj->add_child(nested);

    auto& tc = nested->add<test_component>();

    EXPECT_CALL(tc, on_init()).Times(1);

    EXPECT_CALL(tc, on_update()).Times(5);

    obj->init();

    for (int i = 0; i < 5; i++)
    {
        obj->update();
    }
}
