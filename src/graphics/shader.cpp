#include <glad/gl.h>

#include "graphics/shader.hpp"

#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "graphics/shader_script.hpp"

namespace graphics
{
namespace
{
static inline logger log() { return get_logger("shader"); }
} // namespace

shader::shader() { _id = glCreateProgram(); }

shader::shader(shader&& other)
{
    _id = other._id;
    _shaders = std::move(other._shaders);
    _properties = std::move(other._properties);
    other._id = -1;
}

shader& shader::operator=(shader&& other)
{
    _id = other._id;
    _shaders = std::move(other._shaders);
    _properties = std::move(other._properties);
    other._id = -1;
    return *this;
}

shader::~shader() = default;

void shader::compile()
{
    for (auto& ss : _shaders)
    {
        ss->compile();
        glAttachShader(_id, ss->id());
    }

    glLinkProgram(_id);

    int error_code;
    glGetProgramiv(_id, GL_LINK_STATUS, &error_code);

    if (error_code == GL_FALSE)
    {
        int log_length = 0;
        char msg[ 1024 ];
        glGetProgramInfoLog(_id, 1024, &log_length, msg);
        log()->error("Failed to compile shader {}({}): {}", _id, _name, msg);
        return;
    }
}

void shader::activate()
{
    glUseProgram(_id);
    setup_property_values();
}

void shader::add_shader(std::shared_ptr<shader_script> shader)
{
    _shaders.push_back(shader);
}

void shader::add_shader(std::string_view shader_script_path)
{
    auto ss = std::make_shared<shader_script>(
        std::move(shader_script::from_file(shader_script_path)));
    add_shader(ss);
}

void shader::release_shaders() { _shaders.clear(); }

int shader::id() const { return _id; }

void shader::set_name(std::string name) { _name = std::move(name); }

std::string shader::get_name() const { return _name; }

void shader::set_property(const char* name, std::any value)
{
    set_property(std::string(name), std::move(value));
}

void shader::set_property(std::string_view name, std::any value)
{
    set_property(std::string(name), std::move(value));
}

void shader::set_property(std::string name, std::any value)
{
    _properties[ name ] = std::move(value);
}

void shader::visit_properties(
    std::function<void(std::string_view, const std::any&)> visitor) const
{
    for (const auto& [ name, value ] : _properties)
    {
        visitor(name, value);
    }
}

void shader::setup_property_values() const
{
    for (auto& [ name, v ] : _properties)
    {
        if (!v.has_value())
        {
            continue;
        }

        int id = glGetUniformLocation(_id, name.c_str());
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
        else if (v.type() == typeid(int))
        {
            glUniform1i(id, std::any_cast<int>(v));
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
        else if (v.type() == typeid(unsigned))
        {
            glUniform1ui(id, std::any_cast<unsigned>(v));
        }
        else if (v.type() == typeid(std::tuple<unsigned>))
        {
            auto [ v0 ] = std::any_cast<std::tuple<unsigned>>(v);
            glUniform1ui(id, v0);
        }
        else if (v.type() == typeid(std::tuple<unsigned, unsigned>))
        {
            auto [ v0, v1 ] = std::any_cast<std::tuple<unsigned, unsigned>>(v);
            glUniform2ui(id, v0, v1);
        }
        else if (v.type() == typeid(std::tuple<unsigned, unsigned, unsigned>))
        {
            auto [ v0, v1, v2 ] =
                std::any_cast<std::tuple<unsigned, unsigned, unsigned>>(v);
            glUniform3ui(id, v0, v1, v2);
        }
        else if (v.type() ==
                 typeid(std::tuple<unsigned, unsigned, unsigned, unsigned>))
        {
            auto [ v0, v1, v2, v3 ] = std::any_cast<
                std::tuple<unsigned, unsigned, unsigned, unsigned>>(v);
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
                        name);
        }
    }
}

shader shader::from_file(std::string_view path)
{
    if (!common::file::exists(path))
    {
        log()->error("Shader script {} does not exist", path);
        return shader();
    }

    std::string content = common::file::read_all(path);
    std::vector<std::string> shader_script_paths;

    // tokenize content by newline characters
    std::string::size_type start = 0;
    auto new_start = content.find('\n', start);
    while (new_start != std::string::npos)
    {
        auto line = content.substr(start, new_start - start);
        shader_script_paths.push_back(line);
        start = new_start + 1;
        new_start = content.find('\n', start);
    }

    auto spath = common::filesystem::path(path);
    shader prog;
    for (const auto& sspath : shader_script_paths)
    {
        prog.add_shader(
            (common::filesystem::path(spath.full_path_without_filename()) /
             sspath)
                .full_path());
    }
    prog._name = spath.stem();
    prog._id = glCreateProgram();
    prog.compile();

    return prog;
}
} // namespace graphics
