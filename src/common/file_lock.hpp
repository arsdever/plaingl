#pragma once

#include "common/file.hpp"

namespace common
{
class file_lock
{
public:
    file_lock(file& file)
        : _file(file)
    {
    }

    template <typename... ARGS>
    file_lock(file& file, ARGS&&... args)
        : _file(file)
    {
        _file.open(std::forward<ARGS>(args)...);
    }

    ~file_lock() { _file.close(); }

private:
    file& _file;
};
} // namespace common
