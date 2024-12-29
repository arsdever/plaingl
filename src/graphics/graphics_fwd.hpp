#pragma once

namespace graphics
{
class material;
class mesh;
class shader_script;
class shader;
class font;
class texture;

template <typename R, typename... ARGS>
R gl_convert(ARGS... args);
} // namespace graphics

class graphics_buffer;
class framebuffer;
