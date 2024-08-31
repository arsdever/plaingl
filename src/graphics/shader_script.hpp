#pragma once

namespace graphics
{
enum class shader_script_type
{
    unknown,
    vertex,
    fragment,
    compute
};

class shader_script
{
public:
    shader_script(std::string_view path);
    shader_script(std::string_view path, shader_script_type type);
    shader_script(std::string path, shader_script_type type);

    void update(std::string_view content);

    void compile();

    int id() const;

    static shader_script_type get_type_from_path(std::string_view path);
    static shader_script from_file(std::string_view path);

private:
    shader_script_type _type;
    std::string _path;
    std::string _content;
    int _id { -1 };

    bool _compiled { false };
};
} // namespace graphics
