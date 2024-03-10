#pragma once

class command
{
public:
    virtual ~command() = default;

    virtual void execute() = 0;
};
