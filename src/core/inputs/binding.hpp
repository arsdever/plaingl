#pragma once

#include "common/utils.hpp"

namespace core
{
class binding
{
public:
    template <typename F>
    binding(F updater)
    {
        using ret_t = function_info<F>::return_type;
        _updater = [ this, updater ] { _parameter = updater(); };
    }

    template <typename T>
    T get()
    {
        _updater();
        if constexpr (is_glm_type_v<T>)
        {
            return T(std::any_cast<glm_typecast_t<T, float>>(_parameter));
        }
        else
        {
            return std::any_cast<T>(_parameter);
        }
    }

    std::any get_any() { return _parameter; }

    std::function<void()> _updater;
    std::any _parameter;
};
} // namespace core
