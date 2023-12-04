#pragma once

#include <functional>
#include <tuple>
#include <vector>

template <typename T>
class event
{
public:
    event() = default;

    template <typename... ARGS>
    void operator()(ARGS&&... args)
    {
        for (auto& listener : _listeners)
        {
            listener(std::forward<ARGS...>(args...));
        }
    }

    void operator+=(std::function<T> listener)
    {
        _listeners.push_back(listener);
    }

    bool has_listeners() { return !_listeners.empty(); }

private:
    std::vector<std::function<T>> _listeners;
};
