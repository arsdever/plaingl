/* clang-format off */
#include <glad/gl.h>
#include <GLFW/glfw3.h>
/* clang-format on */

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "light.hpp"

#include "logging.hpp"

namespace
{
static logger log() { return get_logger("light"); }
} // namespace

light::light() { _lights.push_back(this); }

light::~light() { std::erase(_lights, this); }

glm::vec3 light::get_color() const { return _color; }

void light::set_color(glm::vec3 color) { _color = color; }

float light::get_intensity() const { return _intensity; }

void light::set_intensity(float intensity) { _intensity = intensity; }

light::type light::get_type() const { return _light_type; }

void light::set_type(type light_type) { _light_type = light_type; }

transform& light::get_transform() { return _transformation; }

const transform& light::get_transform() const { return _transformation; }

const std::vector<light*>& light::get_all_lights() { return light::_lights; }

std::vector<light*> light::_lights;
