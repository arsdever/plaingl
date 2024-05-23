#pragma once

#include "experimental/window.hpp"
#include "utils.hpp"

namespace experimental
{

class editor_window
    : public window
    , public singleton<editor_window>
{
public:
    ~editor_window();

private:
    editor_window();

    friend singleton_t;
};

} // namespace experimental
