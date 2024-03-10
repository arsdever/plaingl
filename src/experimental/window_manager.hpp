#pragma once

#include <memory>

namespace experimental
{

class window;

class window_manager
{
public:
    static void init();
    static void run();
    static std::shared_ptr<experimental::window> create_window();
};

} // namespace experimental
