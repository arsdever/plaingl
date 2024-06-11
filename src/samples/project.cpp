#include "experimental/project/application_context.hpp"
#include "experimental/project/components/transform.hpp"
#include "experimental/project/game_object.hpp"

int main(int argc, char** argv)
{
    auto app = application_context();

    auto parent_object = game_object::create();
    auto object = game_object::create();

    parent_object->get_transform().set_rotation(
        glm::rotate(glm::identity<glm::dquat>(),
                    glm::radians(90.0),
                    glm::dvec3(1.0, 0.0, 0.0)));

    parent_object->get_transform().set_scale(glm::dvec3(2, 1, 1));

    object->get_transform().set_position(glm::dvec3(0.0, 0.0, 1.0));

    parent_object->set_name("Parent");
    object->set_name("Object in space");
    parent_object->add_child(object);

    glm::mat4 world =
        object->get_transform()
            .get_matrix<components::transform::relation_flag::world>();

    std::cout
        << "\tLocal: "
        << glm::to_string(
               object->get_transform()
                   .get_matrix<components::transform::relation_flag::local>())
        << "\n\tWorld: " << glm::to_string(world) << "\n\tWorld position: "
        << glm::to_string(
               object->get_transform()
                   .get_position<components::transform::relation_flag::world>())
        << "\n\tWorld rotation: "
        << glm::to_string(
               object->get_transform()
                   .get_rotation<components::transform::relation_flag::world>())
        << "\n\tWorld scale: "
        << glm::to_string(
               object->get_transform()
                   .get_scale<components::transform::relation_flag::world>())
        << std::endl;

    return 0;
}
