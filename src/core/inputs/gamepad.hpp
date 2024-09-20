#pragma once

namespace core::inputs
{
class gamepad
{
public:
    enum buttons
    {
        square,
        cross,
        circle,
        triangle,
        left1,
        reft1,
        left2,
        reft2,
        share,
        options,
        left_joystick,
        right_joystick,
        home,
        trackpad,
        arrow_up,
        arrow_right,
        arrow_down,
        arrow_left,
        btn_count
    };

    gamepad(int id);

    bool get_button(buttons btn_code);
    float get_left_trigger();
    float get_right_trigger();
    glm::vec2 get_left_axis();
    glm::vec2 get_right_axis();

protected:
    virtual void update_state();

protected:
    int _joystick_index;
    std::array<char, 15 + sizeof(float) * 6> _state;
};
} // namespace core::inputs
