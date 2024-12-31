#include <stdexcept>

#include "common/main_thread_dispatcher.hpp"

namespace common
{
struct main_thread_dispatcher::impl
{
    std::queue<std::function<void()>> _queue;
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _running { true };
};

void main_thread_dispatcher::initialize()
{
    _instance = std::make_unique<impl>();
}

void main_thread_dispatcher::shutdown() { _instance = nullptr; }

void main_thread_dispatcher::dispatch(std::function<void()> f)
{
    _instance->_queue.push(f);
    _instance->_cv.notify_one();
}

void main_thread_dispatcher::dispatch_sync(std::function<void()> f)
{
    throw std::logic_error("Not implemented");
}

std::function<void()> main_thread_dispatcher::pull_one()
{
    std::lock_guard<std::mutex> lock(_instance->_mutex);
    if (_instance->_queue.empty())
    {
        return {};
    }

    auto f = _instance->_queue.front();
    _instance->_queue.pop();
    return f;
}

void main_thread_dispatcher::run_one()
{
    auto f = pull_one();
    if (f)
    {
        f();
    }
}

void main_thread_dispatcher::run_all()
{
    while (auto f = pull_one())
    {
        f();
    }
}

std::unique_ptr<main_thread_dispatcher::impl>
    main_thread_dispatcher::_instance { nullptr };
} // namespace common
