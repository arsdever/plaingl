#include <glad/gl.h>

#include "shader.hpp"

#include "file.hpp"
#include "logging.hpp"

namespace
{
static inline logger log() { return get_logger("shader"); }
} // namespace

shader::shader(shader_type type)
    : _type(type)
{
}

void shader::init()
{
    if (_status != status::uninitialized)
    {
        log()->warn("Trying to initialize an already initialized shader {}",
                    _id);
        deinit();
    }

    int gl_shader_type = 0;
    switch (_type)
    {
    case shader_type::VERTEX:
    {
        gl_shader_type = GL_VERTEX_SHADER;
        break;
    }
    case shader_type::FRAGMENT:
    {
        gl_shader_type = GL_FRAGMENT_SHADER;
        break;
    }
    case shader_type::COMPUTE:
    {
        gl_shader_type = GL_COMPUTE_SHADER;
        break;
    }
    default:
    {
        break;
    }
    }

    _id = glCreateShader(gl_shader_type);
    _status = status::initialized;
}

void shader::compile()
{
    if (_status == status::uninitialized)
    {
        log()->warn("The shader is not ready for compiling");
        return;
    }

    glCompileShader(_id);

    int error_code;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &error_code);
    if (error_code == GL_FALSE)
    {
        int log_length = 0;
        char msg[ 1024 ];
        glGetShaderInfoLog(_id, 1024, &log_length, msg);
        log()->error("Failed to compile shader {}: {}", _id, msg);
        return;
    }

    _status = status::compiled;
}

void shader::deinit()
{
    if (_status == status::uninitialized)
    {
        return;
    }

    glDeleteShader(_id);
    _id = 0;
    _status = status::uninitialized;
}

void shader::set_source(std::string_view source_code)
{
    const char* data = source_code.data();
    int size = source_code.size();
    glShaderSource(_id, 1, &data, &size);
    _status = status::updated;
}

int shader::id() const { return _id; }

shader shader::from_file(std::string_view path)
{
    std::string_view extension = path.substr(path.find_last_of("."));
    shader_type type = shader_type::VERTEX;
    if (extension == ".frag")
    {
        type = shader_type::FRAGMENT;
    }
    else if (extension == ".compute" || extension == ".shader")
    {
        type = shader_type::COMPUTE;
    }
    shader result(type);

    std::string data = get_file_contents(path);
    result.init();
    result.set_source(data);
    result.compile();
    return result;
}

shader_program::shader_program() = default;

shader_program::shader_program(shader_program&& other)
{
    _status = other._status;
    _id = other._id;
    _shaders = std::move(other._shaders);
    other._id = 0;
    other._status = status::uninitialized;
}

shader_program& shader_program::operator=(shader_program&& other)
{
    _status = other._status;
    _id = other._id;
    _shaders = std::move(other._shaders);
    other._id = 0;
    other._status = status::uninitialized;
    return *this;
}

shader_program::~shader_program()
{
    // no need to manually deinit shaders
    // they will automatically deinit when the vector gets deleted

    deinit();
}

void shader_program::init()
{
    if (_status != status::uninitialized)
    {
        log()->warn(
            "Trying to initialize an already initialized shader program {}",
            _id);
        deinit();
    }

    _id = glCreateProgram();
    _status = status::initialized;
}

void shader_program::link()
{
    if (_status == status::uninitialized)
    {
        log()->warn("The shader program is not ready for linking");
        return;
    }

    glLinkProgram(_id);

    int error_code;
    glGetProgramiv(_id, GL_LINK_STATUS, &error_code);
    if (error_code == GL_FALSE)
    {
        int log_length = 0;
        char msg[ 1024 ];
        glGetProgramInfoLog(_id, 1024, &log_length, msg);
        log()->error("Failed to link program {}: {}", _id, msg);
        return;
    }

    _status = status::linked;
}

void shader_program::use() const
{
    if (_status != status::linked)
    {
        log()->warn("The shader program is not linked");
        return;
    }

    glUseProgram(_id);
}

void shader_program::deinit()
{
    // don't deinit shaders, we only deinit the program
    // the shaders may be reused after

    if (_status == status::uninitialized)
    {
        return;
    }

    glDeleteProgram(_id);
    _id = 0;
    _status = status::uninitialized;
}

void shader_program::add_shader(std::string_view path)
{
    _shaders.push_back(shader::from_file(path));
    glAttachShader(_id, _shaders.back().id());
}

void shader_program::release_shaders() { _shaders.clear(); }

int shader_program::id() const { return _id; }

void shader_program::unuse() { glUseProgram(0); }

glm::mat4 shader_program::_view_matrix;
glm::mat4 shader_program::_projection_matrix;
