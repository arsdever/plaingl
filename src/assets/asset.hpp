#pragma once

#include <string>

class asset
{
public:
    asset(std::string_view id);
    virtual ~asset() = 0;

    std::string id();

private:
    std::string _id;
};
