#pragma once

#include <string>
#include <string_view>

struct string_hash
{
    using is_transparent = void; // enables heterogenous lookup

    std::size_t operator()(const char* char_string) const
    {
        std::hash<std::string_view> hasher;
        return hasher(char_string);
    }
    std::size_t operator()(std::string_view string_view) const
    {
        std::hash<std::string_view> hasher;
        return hasher(string_view);
    }
};
