#pragma once

#include "core/window.hpp"

class profiler : public core::window
{
public:
    profiler();
    ~profiler() override;

protected:
    void initialize();
    void render();

private:
    struct impl;
    std::unique_ptr<impl> _impl;
};
