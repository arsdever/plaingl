#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <spdlog/cfg/argv.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#pragma clang diagnostic pop

namespace spdlog
{
class logger;
}

using logger = std::shared_ptr<spdlog::logger>;

inline logger get_logger(std::string_view logger_name);

inline logger get_logger(std::string_view logger_name)
{
    logger l = spdlog::get(std::string(logger_name));
    if (!l)
    {
        l = spdlog::stdout_color_mt(std::string(logger_name));
    }
    return l;
}

inline void configure_levels(int argc, char** argv)
{
    spdlog::cfg::load_argv_levels(argc, argv);
}
