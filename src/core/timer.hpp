#pragma once

#include "common/event.hpp"

namespace core
{
class timer
{
public:
    timer();
    ~timer();

    void start(std::chrono::duration<double> delay);
    void set_iteration(std::chrono::duration<double> iteration);
    void set_not_repeating();
    void wait();
    void cancel();

    static void single_shot(std::chrono::duration<double> delay,
                            std::function<void()> callback);

    event<void()> tick;

private:
    bool run_and_wait(std::chrono::duration<double> sleep_time);

private:
    std::chrono::steady_clock::time_point _start_time;
    std::chrono::duration<double> _delay { 0 };
    std::optional<std::chrono::duration<double>> _iteration { std::nullopt };

    std::thread _thread;
    std::condition_variable _timer_stopper;
    std::mutex _mutex;
    std::atomic_bool _cancelled { false };
};
} // namespace core
