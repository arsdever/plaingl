#pragma once

#include <unordered_set>

class input_system
{
public:
    static bool is_key_down(int keycode);
    static void set_key_down(int keycode, bool state = true);

private:
    static std::unordered_set<int> _pressed_keys;
};
