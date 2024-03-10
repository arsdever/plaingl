#include <mutex>
#include <thread>

#include "experimental/window_manager.hpp"

#include "experimental/window.hpp"

namespace experimental
{

namespace
{
std::mutex windows_mutex;
std::vector<std::shared_ptr<window>> windows;
} // namespace

void window_manager::init() { }

void window_manager::run()
{
    while (1)
    {
        std::lock_guard<std::mutex> guard(windows_mutex);
        for (auto wnd : windows)
        {
            wnd->update();
        }
    }
}

std::shared_ptr<experimental::window> window_manager::create_window()
{
    std::shared_ptr<experimental::window> wnd =
        std::make_shared<experimental::window>();
    wnd->init();
    wnd->get_events()->close += [](auto ce)
    {
        std::erase_if(windows,
                      [ &ce ](auto wnd) -> bool
        { return wnd.get() == ce.get_sender(); });
    };
    wnd->deactivate();
    std::lock_guard<std::mutex> guard(windows_mutex);
    windows.push_back(wnd);

    return wnd;
}

} // namespace experimental
