#include "components/fps_show_component.hpp"

#include "components/text_component.hpp"
#include "game_clock.hpp"
#include "logging.hpp"

fps_show_component::fps_show_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void fps_show_component::update()
{
    if (get_component<text_component>() == nullptr)
    {
        log()->error("requires text_component");
        return;
    }

    get_component<text_component>()->set_text(
        fmt::format("FPS: {:#6.6} PHYSICS: {:#8}",
                    1.0 / game_clock::delta().count(),
                    1.0 / game_clock::physics_delta().count()));
}
