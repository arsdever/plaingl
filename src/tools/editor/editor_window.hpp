#pragma once

#include "core/window.hpp"

class editor_window : public core::window
{
public:
    editor_window();
    ~editor_window() override;

private:
    void initialize();
    void render();

struct impl;
std::unique_ptr<impl> _impl;
};
