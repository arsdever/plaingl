#pragma once

#include "common/utils.hpp"
#include "graphics/uniform_info.hpp"

namespace graphics
{
class shader_script;

class shader
{
public:
    shader();
    shader(const shader& other) = delete;
    shader(shader&& other);
    shader& operator=(const shader& other) = delete;
    shader& operator=(shader&& other);
    ~shader();

    void compile();
    void activate();

    void add_shader(std::shared_ptr<shader_script> shader);
    void add_shader(std::string_view shader_script_path);
    void release_shaders();

    int id() const;

    void set_name(std::string name);
    std::string get_name() const;

    void set_property(const char* name, std::any value);
    void set_property(std::string_view name, std::any value);
    void set_property(std::string name, std::any value);
    void visit_properties(
        std::function<void(std::string_view, const std::any&)> visitor) const;

    static shader from_file(std::string_view path);

private:
    void resolve_uniforms();
    void setup_property_values() const;

private:
    int _id { 0 };
    std::vector<std::shared_ptr<shader_script>> _shaders;
    std::string _name;
    std::unordered_map<std::string, std::any> _properties;
};
} // namespace graphics
