#include "tools/editor/editor_window.hpp"

#include "common/logging.hpp"
#include "graphics/font.hpp"
#include "project/game_object.hpp"
#include "project/scene.hpp"
#include "renderer/renderer_2d.hpp"

struct editor_window::impl
{
    font _font;
};

editor_window::editor_window()
{
    _impl = std::make_unique<impl>();
    _impl->_font.load("resources/font.ttf", 14);
    on_user_initialize += [ this ](auto) { initialize(); };
}

editor_window::~editor_window() { }

void editor_window::initialize()
{
    set_title("Editor");
    get_events()->render += [ this ](auto) { render(); };
}

void editor_window::render()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const auto line_height = 14;
    const auto line_padding = 2;
    renderer_2d().draw_text({ 5, 19 },
                            _impl->_font,
                            { get_width(), get_height() },
                            scene::get_active_scene()->get_name());

    int element_index = 1;
    scene::get_active_scene()->visit_root_objects(
        [ this, &element_index ](auto obj)
    {
        renderer_2d().draw_text(
            { 5, 19 + (line_height + line_padding) * element_index },
            _impl->_font,
            { get_width(), get_height() },
            std::format(" |-> {}", obj->get_name()));

        ++element_index;
    });
}
