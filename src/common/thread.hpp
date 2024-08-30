#pragma once

void set_thread_name(std::thread& thd, std::string_view name);

void set_thread_priority(std::thread& thd, int priority);

class adjust_timeout_accuracy_guard
{
public:
    adjust_timeout_accuracy_guard();
    ~adjust_timeout_accuracy_guard();
};
