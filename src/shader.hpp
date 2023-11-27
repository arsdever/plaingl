#pragma once

#include <string_view>
#include <vector>

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
    int id() const;

    static shader from_file(std::string_view path);

private:
    status _status = status::uninitialized;
    int _id = 0;
    shader_type _type;
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

private:
    status _status = status::uninitialized;
    int _id = 0;
    std::vector<shader> _shaders;
};
