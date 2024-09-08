#pragma once

#include <glm/vec2.hpp>

#include "core/core_fwd.hpp"

#include "common/event.hpp"

namespace core
{

class window_event
{
public:
    enum class type
    {
        Close,
        Move,
        Resize,
        Render,
        Enter,
        Leave,
        MouseButtonPress,
        MouseButtonRelease,
        MouseButtonClick,
        MouseButtonDoubleClick,
        MouseButtonTripleClick,
        MouseWheel,
        MouseMove,
        KeyPress,
        KeyRepeat,
        KeyRelease,
    };

    window_event(type t, window* sender = nullptr);

    virtual ~window_event();

    type get_type() const;

    window* get_sender() const;

private:
    type _type;
    window* _sender;
};

class render_event : public window_event
{
public:
    render_event(type t, window* sender = nullptr);
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
    input_event(type t, modifiers mod, window* sender = nullptr);

    modifiers get_modifiers() const;

private:
    modifiers _modifiers { modifiers::Unspecified };
};

class single_point_event : public input_event
{
public:
    single_point_event(type t,
                       const glm::vec2& local_pos,
                       const glm::vec2& scene_pos,
                       const glm::vec2& global_pos,
                       int button,
                       int buttons,
                       modifiers mods,
                       window* sender = nullptr);

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

class mouse_event : public single_point_event
{
public:
    mouse_event(type t,
                const glm::vec2& local_pos,
                const glm::vec2& scene_pos,
                const glm::vec2& global_pos,
                int button,
                int buttons,
                modifiers mods,
                window* sender = nullptr);
};

class wheel_event : public single_point_event
{
public:
    wheel_event(type t,
                const glm::vec2& local_pos,
                const glm::vec2& scene_pos,
                const glm::vec2& global_pos,
                int button,
                int buttons,
                modifiers mods,
                glm::vec2 delta,
                bool inverted,
                window* sender = nullptr);

    glm::vec2 get_delta() const;
    bool get_is_inverted() const;

private:
    glm::vec2 _delta { 0, 0 };
    bool _is_inverted { false };
};

class enter_event : public single_point_event
{
public:
    enter_event(type t,
                const glm::vec2& local_pos,
                const glm::vec2& scene_pos,
                const glm::vec2& global_pos,
                int button,
                int buttons,
                modifiers mods,
                window* sender = nullptr);
};

class leave_event : public window_event
{
public:
    leave_event(window* sender = nullptr);
};

class close_event : public window_event
{
public:
    close_event(window* sender = nullptr);
};

class resize_event : public window_event
{
public:
    resize_event(glm::vec2 old_size, glm::vec2 size, window* sender = nullptr);

    glm::vec2 get_old_size() const;
    glm::vec2 get_new_size() const;

private:
    glm::vec2 _old_size;
    glm::vec2 _new_size;
};

class move_event : public window_event
{
public:
    move_event(glm::vec2 old_position,
               glm::vec2 position,
               window* sender = nullptr);

    glm::vec2 get_old_position() const;
    glm::vec2 get_new_position() const;

private:
    glm::vec2 _old_position;
    glm::vec2 _new_position;
};

class key_event : public input_event
{
public:
    key_event(type t,
              int scancode,
              int key,
              modifiers mods,
              bool repeated = false,
              window* sender = nullptr);

    int get_scancode() const;
    int get_key() const;
    bool get_is_repeated() const;

private:
    int _scancode;
    int _key;
    bool _is_repeated { false };
};

class window_events
{
public:
    float get_drag_distance() const;
    void set_drag_distance(std::optional<float> distance);
    static void set_default_drag_distance(float distance);

    event<void(close_event)> close;
    event<void(resize_event)> resize;
    event<void(render_event)> render;
    event<void(move_event)> move;

    event<void(enter_event)> enter;
    event<void(window_event)> leave;

    event<void(mouse_event)> mouse_press;
    event<void(mouse_event)> mouse_release;
    event<void(mouse_event)> mouse_click;
    event<void(mouse_event)> mouse_double_click;
    event<void(mouse_event)> mouse_triple_click;

    event<void(mouse_event)> mouse_move;

    event<void(wheel_event)> mouse_scroll;

    event<void(key_event)> key_press;
    event<void(key_event)> key_release;
    event<void(key_event)> key_repeat;

private:
    bool _is_drag = false;
    std::optional<float> _drag_distance;

    static float _default_drag_distance;
};

} // namespace core
