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

    std::string data = get_file_contents(path);
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
        switch (property._type)
        {
        case uniform_info::type::property_type_1f:
        {
            auto [ f1 ] = std::any_cast<std::tuple<float>>(property._value);
            glUniform1f(id, f1);
            break;
        }
        case uniform_info::type::property_type_2f:
        {
            auto [ f1, f2 ] =
                std::any_cast<std::tuple<float, float>>(property._value);
            glUniform2f(id, f1, f2);
            break;
        }
        case uniform_info::type::property_type_3f:
        {
            auto [ f1, f2, f3 ] =
                std::any_cast<std::tuple<float, float, float>>(property._value);
            glUniform3f(id, f1, f2, f3);
            break;
        }
        case uniform_info::type::property_type_4f:
        {
            auto [ f1, f2, f3, f4 ] =
                std::any_cast<std::tuple<float, float, float, float>>(
                    property._value);
            glUniform4f(id, f1, f2, f3, f4);
            break;
        }
        case uniform_info::type::property_type_sampler2D:
        case uniform_info::type::property_type_1i:
        {
            auto [ i1 ] = std::any_cast<std::tuple<int>>(property._value);
            glUniform1i(id, i1);
            break;
        }
        case uniform_info::type::property_type_2i:
        {
            auto [ i1, i2 ] =
                std::any_cast<std::tuple<int, int>>(property._value);
            glUniform2i(id, i1, i2);
            break;
        }
        case uniform_info::type::property_type_3i:
        {
            auto [ i1, i2, i3 ] =
                std::any_cast<std::tuple<int, int, int>>(property._value);
            glUniform3i(id, i1, i2, i3);
            break;
        }
        case uniform_info::type::property_type_4i:
        {
            auto [ i1, i2, i3, i4 ] =
                std::any_cast<std::tuple<int, int, int, int>>(property._value);
            glUniform4i(id, i1, i2, i3, i4);
            break;
        }
        case uniform_info::type::property_type_1ui:
        {
            auto [ ui1 ] = std::any_cast<std::tuple<unsigned>>(property._value);
            glUniform1ui(id, ui1);
            break;
        }
        case uniform_info::type::property_type_2ui:
        {
            auto [ ui1, ui2 ] =
                std::any_cast<std::tuple<unsigned, unsigned>>(property._value);
            glUniform2ui(id, ui1, ui2);
            break;
        }
        case uniform_info::type::property_type_3ui:
        {
            auto [ ui1, ui2, ui3 ] =
                std::any_cast<std::tuple<unsigned, unsigned, unsigned>>(
                    property._value);
            glUniform3ui(id, ui1, ui2, ui3);
            break;
        }
        case uniform_info::type::property_type_4ui:
        {
            auto [ ui1, ui2, ui3, ui4 ] = std::any_cast<
                std::tuple<unsigned, unsigned, unsigned, unsigned>>(
                property._value);
            glUniform4ui(id, ui1, ui2, ui3, ui4);
            break;
        }
        case uniform_info::type::property_type_mat2:
        {
            auto [ matrix ] =
                std::any_cast<std::tuple<glm::mat2>>(property._value);
            glUniformMatrix2fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
            break;
        }
        case uniform_info::type::property_type_mat3:
        {
            auto [ matrix ] =
                std::any_cast<std::tuple<glm::mat3>>(property._value);
            glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
            break;
        }
        case uniform_info::type::property_type_mat4:
        {
            auto [ matrix ] =
                std::any_cast<std::tuple<glm::mat4>>(property._value);
            glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
            break;
        }
        default: break;
        }
    }

    if (camera::active_camera() && _name_property_map.contains("vp_matrix"))
    {
        glUniformMatrix4fv(
            _name_property_map.find("vp_matrix")->second._index,
            1,
            GL_FALSE,
            glm::value_ptr(camera::active_camera()->vp_matrix()));
    }
}

glm::mat4 shader_program::_view_matrix;
glm::mat4 shader_program::_projection_matrix;
