#pragma once

#include <glm/detail/qualifier.hpp>
#include <nlohmann/json.hpp>

namespace serialization::utilities
{
enum class json_output_format
{
    object,
    array
};

template <json_output_format F = json_output_format::array,
          int N,
          typename T,
          glm::qualifier Q>
nlohmann::json to_json(const glm::vec<N, T, Q>& v)
{
    if constexpr (F == json_output_format::object)
    {
        return nlohmann::json { { "x", v.x }, { "y", v.y }, { "z", v.z } };
    }
    else
    {
        return nlohmann::json::array({ v.x, v.y, v.z });
    }
}

template <json_output_format F = json_output_format::array,
          typename T,
          glm::qualifier Q>
nlohmann::json to_json(const glm::qua<T, Q>& q)
{
    if constexpr (F == json_output_format::object)
    {
        return nlohmann::json {
            { "x", q.x }, { "y", q.y }, { "z", q.z }, { "w", q.w }
        };
    }
    else
    {
        return nlohmann::json::array({ q.x, q.y, q.z, q.w });
    }
}
} // namespace serialization::utilities
