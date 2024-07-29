#include "tools/editor/editor_window.hpp"

#include "common/logging.hpp"
#include "core/input_system.hpp"
#include "core/window_events.hpp"
#include "graphics/font.hpp"
#include "project/game_object.hpp"
#include "project/project_manager.hpp"
#include "project/scene.hpp"
#include "renderer/renderer_2d.hpp"

namespace
{
logger log() { return get_logger("editor"); }
} // namespace

struct editor_window::impl
{
    font font;
    std::unordered_map<glm::vec4, std::shared_ptr<object>> object_map;

    struct clickable_region
    {
        glm::vec2 position;
        glm::vec2 size;
        bool hover;
        bool selected;

        bool contains(const glm::vec2& p) const
        {
            return p.x >= position.x && p.x <= position.x + size.x &&
                   p.y >= position.y && p.y <= position.y + size.y;
        }
    };

    std::unordered_map<std::shared_ptr<object>, clickable_region> regions;
};

editor_window::editor_window()
{
    _impl = std::make_unique<impl>();
    _impl->font.load("resources/font.ttf", 14);
    on_user_initialize += [ this ](auto) { initialize(); };
}

editor_window::~editor_window() { }

void editor_window::initialize()
{
    set_title("Editor");
    get_events()->render += [ this ](auto) { render(); };
    get_events()->mouse_move += [ this ](auto me)
    {
        for (auto& region : _impl->regions)
        {
            region.second.hover =
                region.second.contains(me.get_local_position());
        }
    };
    get_events()->mouse_click += [ this ](auto me)
    {
        std::vector<std::shared_ptr<object>> selected_objects;
        for (auto& region : _impl->regions)
        {
            if (!(me.get_modifiers() & core::input_event::modifiers::Control))
            {
                region.second.selected = false;
            }

            if (region.second.contains(me.get_local_position()))
            {
                region.second.selected = !region.second.selected;
            }

            if (region.second.selected)
            {
                selected_objects.push_back(region.first);
            }
        }

        project_manager::set_object_selection(selected_objects);
    };
}

void editor_window::render()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const auto line_height = 14;
    const glm::vec2 line_padding { 0, 2 };
    glm::vec2 offset {};
    const auto scene_name_size =
        _impl->font.get_text_size(scene::get_active_scene()->get_name());
    if (!_impl->regions.contains(scene::get_active_scene()))
    {
        _impl->regions[ scene::get_active_scene() ] = { offset,
                                                        scene_name_size };
    }

    renderer_2d().draw_rect(offset,
                            offset + scene_name_size,
                            { get_width(), get_height() },
                            0,
                            glm::vec4(0),
                            _impl->regions[ scene::get_active_scene() ].selected
                                ? glm::vec4(.6f, .5f, 0, 1)
                            : _impl->regions[ scene::get_active_scene() ].hover
                                ? glm::vec4(.4f, .3f, 0, 1)
                                : glm::vec4(.3f, .2f, 0, 1));
    renderer_2d().draw_text(offset + glm::vec2 { 0, line_height },
                            _impl->font,
                            { get_width(), get_height() },
                            scene::get_active_scene()->get_name());
    offset.y += scene_name_size.y;
    offset.x += 20;
    scene::get_active_scene()->visit_root_objects(
        [ this, line_padding, &offset ](auto obj)
    {
        auto str = std::format("[ ] |-> {}", obj->get_name());
        auto name_size = _impl->font.get_text_size(str);
        if (!_impl->regions.contains(obj))
        {
            _impl->regions[ obj ] = { offset, name_size };
        }

        renderer_2d().draw_rect(
            offset,
            offset + name_size,
            { get_width(), get_height() },
            0,
            glm::vec4(0),
            _impl->regions[ obj ].selected ? glm::vec4(.5f, .6f, 0, 1)
            : _impl->regions[ obj ].hover  ? glm::vec4(.3f, .4f, 0, 1)
                                           : glm::vec4(.2f, .3f, 0, 1));
        renderer_2d().draw_text(offset + glm::vec2 { 0, line_height },
                                _impl->font,
                                { get_width(), get_height() },
                                str);
        offset.y += name_size.y;
    });
}
