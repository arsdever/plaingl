#pragma once

#include "common/utils.hpp"

namespace core
{
class library
{
public:
    library(std::string path);
    library(const library&) = delete;
    library& operator=(const library&) = delete;
    library(library&&);
    library& operator=(library&&);
    ~library();

    void lazy_load();
    void release();

    template <typename T>
    function_info<T>::type resolve(std::string_view name)
    {
        auto* const proc_address = resolve(name);
        return reinterpret_cast<function_info<T>::type>(proc_address);
    }

private:
    using ftype = long long (*)();
    ftype resolve(std::string_view name);

private:
    struct impl;
    std::unique_ptr<impl> _impl;
};
} // namespace core
