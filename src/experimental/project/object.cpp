#include <string>

#include <entt/entt.hpp>

#include "object.hpp"

object::object() = default;

object::~object() = default;

size_t object::id() const { return _id; }
