#pragma once

#include <string_view>

class asset_loader
{
public:
    virtual ~asset_loader() = 0;
    virtual void load(std::string_view path) = 0;
};

inline asset_loader::~asset_loader() = default;
