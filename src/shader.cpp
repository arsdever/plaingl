#include "shader.hpp"

#include "camera.hpp"
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
        log()->error("Failed to compile shader {}({}): {}", _id, _path, msg);
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

void shader::set_path(std::string_view path) { _path = path; }

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

    std::string data = file::read_all(path);
    result.set_path(path);
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
    _properties = std::move(other._properties);
    _name_property_map = std::move(other._name_property_map);
    other._id = 0;
    other._status = status::uninitialized;
}

shader_program& shader_program::operator=(shader_program&& other)
{
    _status = other._status;
    _id = other._id;
    _shaders = std::move(other._shaders);
    _properties = std::move(other._properties);
    _name_property_map = std::move(other._name_property_map);
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
    resolve_uniforms();
}

void shader_program::use() const
{
    if (_status != status::linked)
    {
        log()->warn("The shader program is not linked");
        return;
    }

    glUseProgram(_id);
    setup_property_values();
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

bool shader_program::linked() const { return _status == status::linked; }

void shader_program::unuse() { glUseProgram(0); }

void shader_program::set_uniform(std::string_view name, std::any value)
{
    auto iterator = _name_property_map.find(name);
    if (iterator != _name_property_map.end())
    {
        iterator->second._value = std::move(value);
    }
}

void shader_program::resolve_uniforms()
{
    use();
    _properties.clear();
    _name_property_map.clear();
    int uniform_count = 0;
    glGetProgramiv(id(), GL_ACTIVE_UNIFORMS, &uniform_count);
    if (uniform_count == 0)
    {
        return;
    }

    std::string buffer;
    int length;
    int size;
    unsigned type;
    buffer.resize(512);
    _properties.resize(uniform_count);
    for (int i = 0; i < uniform_count; ++i)
    {
        glGetActiveUniform(id(), i, 512, &length, &size, &type, buffer.data());
        _properties[ i ]._name = buffer;
        _properties[ i ]._name.resize(length);
        _properties[ i ]._index = i;
        _properties[ i ]._size = size;
        _properties[ i ]._type = uniform_info::type(type);
        // TODO: verify emplace did add element
        _name_property_map.try_emplace(_properties[ i ]._name,
                                       _properties[ i ]);
    }
    shader_program::unuse();
}

void shader_program::setup_property_values() const
{
    for (auto& property : _properties)
    {
        if (!property._value.has_value())
        {
            continue;
        }

        int id = property._index;
        const auto& v = property._value;
        if (v.type() == typeid(float))
        {
            glUniform1f(id, std::any_cast<float>(v));
        }
        else if (v.type() == typeid(std::tuple<float>))
        {
            auto [ v0 ] = std::any_cast<std::tuple<float>>(v);
            glUniform1f(id, v0);
        }
        else if (v.type() == typeid(std::tuple<float, float>))
        {
            auto [ v0, v1 ] = std::any_cast<std::tuple<float, float>>(v);
            glUniform2f(id, v0, v1);
        }
        else if (v.type() == typeid(std::tuple<float, float, float>))
        {
            auto [ v0, v1, v2 ] =
                std::any_cast<std::tuple<float, float, float>>(v);
            glUniform3f(id, v0, v1, v2);
        }
        else if (v.type() == typeid(std::tuple<float, float, float, float>))
        {
            auto [ v0, v1, v2, v3 ] =
                std::any_cast<std::tuple<float, float, float, float>>(v);
            glUniform4f(id, v0, v1, v2, v3);
        }
        else if (v.type() == typeid(std::tuple<int>))
        {
            auto [ v0 ] = std::any_cast<std::tuple<int>>(v);
            glUniform1i(id, v0);
        }
        else if (v.type() == typeid(std::tuple<int, int>))
        {
            auto [ v0, v1 ] = std::any_cast<std::tuple<int, int>>(v);
            glUniform2i(id, v0, v1);
        }
        else if (v.type() == typeid(std::tuple<int, int, int>))
        {
            auto [ v0, v1, v2 ] = std::any_cast<std::tuple<int, int, int>>(v);
            glUniform3i(id, v0, v1, v2);
        }
        else if (v.type() == typeid(std::tuple<int, int, int, int>))
        {
            auto [ v0, v1, v2, v3 ] =
                std::any_cast<std::tuple<int, int, int, int>>(v);
            glUniform4i(id, v0, v1, v2, v3);
        }
        else if (v.type() == typeid(std::tuple<unsigned int>))
        {
            auto [ v0 ] = std::any_cast<std::tuple<unsigned int>>(v);
            glUniform1ui(id, v0);
        }
        else if (v.type() == typeid(std::tuple<unsigned int, unsigned int>))
        {
            auto [ v0, v1 ] =
                std::any_cast<std::tuple<unsigned int, unsigned int>>(v);
            glUniform2ui(id, v0, v1);
        }
        else if (v.type() ==
                 typeid(std::tuple<unsigned int, unsigned int, unsigned int>))
        {
            auto [ v0, v1, v2 ] = std::any_cast<
                std::tuple<unsigned int, unsigned int, unsigned int>>(v);
            glUniform3ui(id, v0, v1, v2);
        }
        else if (v.type() == typeid(std::tuple<unsigned int,
                                               unsigned int,
                                               unsigned int,
                                               unsigned int>))
        {
            auto [ v0, v1, v2, v3 ] =
                std::any_cast<std::tuple<unsigned int,
                                         unsigned int,
                                         unsigned int,
                                         unsigned int>>(v);
            glUniform4ui(id, v0, v1, v2, v3);
        }
        else if (v.type() == typeid(glm::vec1))
        {
            glUniform1f(id, std::any_cast<glm::vec1>(v).x);
        }
        else if (v.type() == typeid(glm::vec2))
        {
            glUniform2f(id,
                        std::any_cast<glm::vec2>(v).x,
                        std::any_cast<glm::vec2>(v).y);
        }
        else if (v.type() == typeid(glm::vec3))
        {
            glUniform3f(id,
                        std::any_cast<glm::vec3>(v).x,
                        std::any_cast<glm::vec3>(v).y,
                        std::any_cast<glm::vec3>(v).z);
        }
        else if (v.type() == typeid(glm::vec4))
        {
            glUniform4f(id,
                        std::any_cast<glm::vec4>(v).x,
                        std::any_cast<glm::vec4>(v).y,
                        std::any_cast<glm::vec4>(v).z,
                        std::any_cast<glm::vec4>(v).w);
        }
        else if (v.type() == typeid(glm::uvec1))
        {
            glUniform1ui(id, std::any_cast<glm::uvec1>(v).x);
        }
        else if (v.type() == typeid(glm::uvec2))
        {
            glUniform2ui(id,
                         std::any_cast<glm::uvec2>(v).x,
                         std::any_cast<glm::uvec2>(v).y);
        }
        else if (v.type() == typeid(glm::uvec3))
        {
            glUniform3ui(id,
                         std::any_cast<glm::uvec3>(v).x,
                         std::any_cast<glm::uvec3>(v).y,
                         std::any_cast<glm::uvec3>(v).z);
        }
        else if (v.type() == typeid(glm::uvec4))
        {
            glUniform4ui(id,
                         std::any_cast<glm::uvec4>(v).x,
                         std::any_cast<glm::uvec4>(v).y,
                         std::any_cast<glm::uvec4>(v).z,
                         std::any_cast<glm::uvec4>(v).w);
        }
        else if (v.type() == typeid(glm::ivec1))
        {
            glUniform1i(id, std::any_cast<glm::ivec1>(v).x);
        }
        else if (v.type() == typeid(glm::ivec2))
        {
            glUniform2i(id,
                        std::any_cast<glm::ivec2>(v).x,
                        std::any_cast<glm::ivec2>(v).y);
        }
        else if (v.type() == typeid(glm::ivec3))
        {
            glUniform3i(id,
                        std::any_cast<glm::ivec3>(v).x,
                        std::any_cast<glm::ivec3>(v).y,
                        std::any_cast<glm::ivec3>(v).z);
        }
        else if (v.type() == typeid(glm::ivec4))
        {
            glUniform4i(id,
                        std::any_cast<glm::ivec4>(v).x,
                        std::any_cast<glm::ivec4>(v).y,
                        std::any_cast<glm::ivec4>(v).z,
                        std::any_cast<glm::ivec4>(v).w);
        }
        else if (v.type() == typeid(std::tuple<glm::mat2>))
        {
            auto [ value ] = std::any_cast<std::tuple<glm::mat2>>(v);
            glUniformMatrix2fv(id, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if (v.type() == typeid(std::tuple<glm::mat3>))
        {
            auto [ value ] = std::any_cast<std::tuple<glm::mat3>>(v);
            glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if (v.type() == typeid(std::tuple<glm::mat4>))
        {
            auto [ value ] = std::any_cast<std::tuple<glm::mat4>>(v);
            glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value));
        }
        else if (v.type() == typeid(glm::mat2))
        {
            glUniformMatrix2fv(
                id, 1, GL_FALSE, glm::value_ptr(std::any_cast<glm::mat2>(v)));
        }
        else if (v.type() == typeid(glm::mat3))
        {
            glUniformMatrix3fv(
                id, 1, GL_FALSE, glm::value_ptr(std::any_cast<glm::mat3>(v)));
        }
        else if (v.type() == typeid(glm::mat4))
        {
            glUniformMatrix4fv(
                id, 1, GL_FALSE, glm::value_ptr(std::any_cast<glm::mat4>(v)));
        }
        else
        {
            log()->warn("Unknown uniform type '{}' specified for property {}",
                        v.type().name(),
                        property._name);
        }
    }
}

glm::mat4 shader_program::_view_matrix;
glm::mat4 shader_program::_projection_matrix;
