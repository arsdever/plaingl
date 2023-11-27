#pragma once

#include <string_view>
#include <thread>

void set_thread_name(std::thread& thd, std::string_view name);
