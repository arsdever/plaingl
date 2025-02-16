#pragma once

namespace common
{
class main_thread_dispatcher
{
public:
    static void initialize();
    static void shutdown();
    static void dispatch(std::function<void()> f);
    static void dispatch_sync(std::function<void()> f);

    static std::function<void()> pull_one();
    static void run_one();
    static void run_all();
    static void wait_task_available();

    static bool is_main_thread();

private:
    struct impl;
    static std::unique_ptr<impl> _instance;
};
} // namespace common
