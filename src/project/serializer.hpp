#pragma once

enum class serializer_type
{
    json,
};

template <serializer_type T>
class serializer;

#include "project/serializer_json.hpp"

using json_serializer = serializer<serializer_type::json>;
