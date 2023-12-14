#include "viewport.hpp"

#include "logging.hpp"

namespace
{
static inline logger log() { return get_logger("viewport"); }
} // namespace

size_t viewport::width() const { return _resolution.x; }

size_t viewport::height() const { return _resolution.y; }

glm::uvec2 viewport::resolution() const { return _resolution; }

void viewport::resize(size_t width, size_t height)
{
    resize(glm::uvec2 { width, height });
}

void viewport::resize(glm::uvec2 size) { _resolution = size; }
