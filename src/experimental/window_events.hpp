#pragma once

#include <memory>
#include <optional>

#include <glm/vec2.hpp>

#include "event.hpp"

namespace experimental
{

class window;

class window_event
{
public:
    enum class type
    {
        Close,
        Move,
        Resize,
        MouseButtonPress,
        MouseButtonRelease,
        MouseButtonDoubleClick,
        MouseButtonTripleClick,
        MouseMove,
    };

    window_event(type t);

    virtual ~window_event();

    type get_type() const;

private:
    type _type;
};

class input_event : public window_event
{
public:
    enum modifiers
    {
        Unspecified = 0,
        Shift = 1,
        Control = 2,
        Alt = 4,
    };

public:
    input_event(type t, modifiers mod);

private:
    modifiers _modifiers { modifiers::Unspecified };
};

class mouse_event : public input_event
{
public:
    mouse_event(type t,
                const glm::vec2& local_pos,
                const glm::vec2& scene_pos,
                const glm::vec2& global_pos,
                int button,
                int buttons,
                modifiers mods);

    int get_button() const;
    int get_buttons() const;
    glm::vec2 get_local_position() const;
    glm::vec2 get_scene_position() const;
    glm::vec2 get_global_position() const;

private:
    glm::vec2 _local_pos { 0, 0 };
    glm::vec2 _scene_pos { 0, 0 };
    glm::vec2 _global_pos { 0, 0 };
    int _button { 0 };
    int _buttons { 0 };
};

class window_events
{
public:
    float get_drag_distance() const;
    void set_drag_distance(std::optional<float> distance);
    static void set_default_drag_distance(float distance);

    event<void(mouse_event)> mouse_press;
    event<void(mouse_event)> mouse_right_press;
    event<void(mouse_event)> mouse_middle_press;
    event<void(mouse_event)> mouse_click;
    event<void(mouse_event)> mouse_double_click;
    event<void(mouse_event)> mouse_right_click;
    event<void(mouse_event)> mouse_double_right_click;
    event<void(mouse_event)> mouse_middle_click;
    event<void(mouse_event)> mouse_release;
    event<void(mouse_event)> mouse_right_release;
    event<void(mouse_event)> mouse_middle_release;
    event<void(mouse_event)> mouse_move;
    event<void(mouse_event)> mouse_drag_drop_start;
    event<void(mouse_event)> mouse_drag_drop_move;
    event<void(mouse_event)> mouse_drag_drop_end;
    event<void(mouse_event)> mouse_enter;
    event<void(mouse_event)> mouse_leave;
    event<void(mouse_event)> mouse_scroll;

private:
    bool _is_drag = false;
    std::optional<float> _drag_distance;

    static float _default_drag_distance;
};

} // namespace experimental
