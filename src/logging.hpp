#pragma once

#include <memory>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#pragma clang diagnostic pop

namespace spdlog
{
class logger;
}

using logger = std::shared_ptr<spdlog::logger>;

logger get_logger(std::string_view logger_name);

logger get_logger(std::string_view logger_name)
{
    logger l = spdlog::get(std::string(logger_name));
    if (!l)
    {
        l = spdlog::stdout_color_mt(std::string(logger_name));
    }
    return l;
}
