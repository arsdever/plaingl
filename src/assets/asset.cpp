#include "assets/asset.hpp"

asset::asset(std::string_view id)
    : _id(id)
{
}

asset::~asset() = default;

std::string asset::id() { return _id; }
