#pragma once

#include <format>

#include <nlohmann/json.hpp>

#include "project/serializer.hpp"

template <>
class serializer<serializer_type::json>
{
public:
    serializer() { _root = nlohmann::json(); };

    void start_object() { current = nlohmann::json(); }

    template <typename T>
    void property(const std::string& key, const T& value)
    {
        current[ key ] = value;
    }

    void add_component(nlohmann::json component_object)
    {
        current[ "components" ] += component_object;
    }

    void finish_object() { _root[ "objects" ] += current; }

    const nlohmann::json& root() { return _root; }

private:
    nlohmann::json _root;
    nlohmann::json current;
};
