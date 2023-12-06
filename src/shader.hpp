#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include <glm/ext.hpp>

#include "uniform_info.hpp"
#include "utils.hpp"

enum class shader_type
{
    VERTEX,
    FRAGMENT,
    COMPUTE
};

class shader
{
public:
    enum status
    {
        uninitialized,
        initialized,
        updated,
        compiled,
    };

public:
    shader(shader_type type);

    void init();
    void compile();
    void deinit();
    void set_source(std::string_view source_code);
    void set_path(std::string_view path);
    int id() const;

    static shader from_file(std::string_view path);

private:
    status _status = status::uninitialized;
    int _id = 0;
    shader_type _type;
    std::string _path = "unknown";
};

class shader_program
{
public:
    enum status
    {
        uninitialized,
        initialized,
        updated,
        linked,
    };

public:
    shader_program();
    shader_program(const shader_program& other) = delete;
    shader_program(shader_program&& other);
    shader_program& operator=(const shader_program& other) = delete;
    shader_program& operator=(shader_program&& other);
    ~shader_program();

    void init();
    void link();
    void use() const;
    void deinit();
    void add_shader(std::string_view path);
    void release_shaders();
    int id() const;

    static void unuse();

    void set_uniform(std::string_view name, std::any value);

private:
    void resolve_uniforms();
    void setup_property_values() const;

private:
    status _status = status::uninitialized;
    int _id = 0;
    std::vector<shader> _shaders;
    static glm::mat4 _view_matrix;
    static glm::mat4 _projection_matrix;
    std::vector<uniform_info> _properties;
    std::unordered_map<std::string, uniform_info&, string_hash, std::equal_to<>>
        _name_property_map;
};
