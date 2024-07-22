#pragma once

namespace core
{
class command
{
public:
    virtual ~command() = default;

    virtual void execute() = 0;
};
} // namespace core
