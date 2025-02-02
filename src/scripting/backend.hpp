#pragma once

#include "common/common_fwd.hpp"
#include "scripting/scripting_fwd.hpp"

namespace scripting
{
class backend
{
public:
    ~backend();

    static void initialize();
    static void shutdown();

    static void load_script(std::string_view script_path);

private:
    backend();
    static std::unique_ptr<backend> _instance;

    struct impl;
    std::unique_ptr<impl> _impl;
};
} // namespace scripting
