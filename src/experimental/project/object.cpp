#include <string>

#include <entt/entt.hpp>

#include "object.hpp"

object::object() = default;

object::~object() = default;

std::string object::name() const { return _name; }

void object::set_name(const std::string& name) { _name = name; }

size_t object::id() const { return _id; }
