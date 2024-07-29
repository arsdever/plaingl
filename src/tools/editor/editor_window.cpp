#include "tools/editor/editor_window.hpp"

#include "common/logging.hpp"
#include "graphics/font.hpp"
#include "project/game_object.hpp"
#include "project/scene.hpp"
#include "renderer/renderer_2d.hpp"

struct editor_window::impl
{
    font _font;
    std::unordered_map<glm::vec4, std::shared_ptr<object>> _object_map;
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
    const glm::vec2 line_padding { 0, 2 };
    glm::vec2 offset {};
    const auto scene_name_size =
        _impl->_font.get_text_size(scene::get_active_scene()->get_name());
    renderer_2d().draw_rect(offset,
                            offset + scene_name_size,
                            { get_width(), get_height() },
                            0,
                            glm::vec4(0),
                            glm::vec4(.3f, .2f, 0, 1));
    renderer_2d().draw_text(offset + glm::vec2 { 0, line_height },
                            _impl->_font,
                            { get_width(), get_height() },
                            scene::get_active_scene()->get_name());

    offset.y += scene_name_size.y;
    offset.x += 20;
    scene::get_active_scene()->visit_root_objects(
        [ this, line_padding, &offset ](auto obj)
    {
        auto str = std::format("[ ] |-> {}", obj->get_name());
        auto name_size = _impl->_font.get_text_size(str);
        renderer_2d().draw_rect(offset,
                                offset + name_size,
                                { get_width(), get_height() },
                                0,
                                glm::vec4(0),
                                glm::vec4(.2f, .3f, 0, 0));
        renderer_2d().draw_text(offset + glm::vec2 { 0, line_height },
                                _impl->_font,
                                { get_width(), get_height() },
                                str);

        offset.y += name_size.y;
    });
}
