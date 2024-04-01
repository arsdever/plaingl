#pragma once

#include <unordered_map>

class feature_flags
{
public:
    enum class flag_name
    {
        load_fbx_as_scene,
        multisampling
    };

public:
    inline static bool set_flag(flag_name fl, bool value = true)
    {
        auto flag_it = _flags.find(fl);
        if (flag_it == _flags.end())
        {
            _flags[ fl ] = value;
            return false;
        }

        bool old = flag_it->second;
        flag_it->second = value;
        return old;
    }

    inline static bool get_flag(flag_name fl)
    {
        auto flag_it = _flags.find(fl);
        if (flag_it == _flags.end())
        {
            return _flags[ fl ] = false;
        }
        return flag_it->second;
    }

private:
    inline static std::unordered_map<flag_name, bool> _flags {};
};
