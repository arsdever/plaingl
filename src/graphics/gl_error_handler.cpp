#include <glad/gl.h>

#include "common/logging.hpp"

namespace
{
static inline logger log() { return get_logger("opengl"); }
} // namespace

void gl_error_handler(unsigned source,
                      unsigned type,
                      unsigned id,
                      unsigned severity,
                      int length,
                      const char* message,
                      const void* user_parameter)
{
    // SOURCE: https://learnopengl.com/In-Practice/Debugging
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    (void)user_parameter;

    spdlog::level::level_enum log_level = spdlog::level::off;
    std::string_view source_string;
    std::string_view type_string;
    std::string_view message_string(message, length);

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH: log_level = spdlog::level::critical; break;
    case GL_DEBUG_SEVERITY_MEDIUM: log_level = spdlog::level::err; break;
    case GL_DEBUG_SEVERITY_LOW: log_level = spdlog::level::warn; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: log_level = spdlog::level::info; break;
    }

    switch (source)
    {
    case GL_DEBUG_SOURCE_API: source_string = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_string = "Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_string = "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: source_string = "Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION: source_string = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER: log()->error("Source: Other"); break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: type_string = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        type_string = "Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        type_string = "Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY: type_string = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: type_string = "Performance"; break;
    case GL_DEBUG_TYPE_MARKER: type_string = "Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP: type_string = "Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP: type_string = "Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER: type_string = "Other"; break;
    }

    log()->log(log_level,
               "({}) \"{}\" | {}: {}",
               source_string,
               id,
               type_string,
               message_string);
}
