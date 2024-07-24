#pragma once

#include "experimental/window.hpp"

class profiler : public experimental::window
{
public:
    profiler();
    ~profiler();

protected:
    void initialize();
    void render();

private:
    struct impl;
    std::unique_ptr<impl> _impl;
};
