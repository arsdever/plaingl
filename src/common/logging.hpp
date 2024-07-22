#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <spdlog/cfg/argv.h>
#include <spdlog/sinks/dist_sink.h>
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
    auto l = spdlog::get(std::string(logger_name));
    if (!l)
    {
        l = spdlog::default_logger()->clone(std::string(logger_name));
    }

    return l;
}

inline void configure_logging(int argc, char** argv)
{
    spdlog::cfg::load_argv_levels(argc, argv);
    auto dist_sink = std::make_shared<spdlog::sinks::dist_sink_st>();
    dist_sink->add_sink(
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    auto& sinks = spdlog::default_logger()->sinks();
    sinks.clear();
    sinks.push_back(dist_sink);
}
