#pragma once

#include <unordered_map>

#include "component.hpp"

struct GLFWwindow;
class font;
class material;

class text_renderer_component : public component
{
public:
    text_renderer_component(game_object* parent);

    void set_font(font* ttf);
    font* get_font();

    void set_material(material* mat);
    material* get_material();

    void init() override;
    void update() override;
    void deinit() override;

    static constexpr std::string_view class_type_id = "text_renderer_component";

private:
    material* _material = nullptr;
    font* _font = nullptr;
    std::unordered_map<GLFWwindow*, unsigned int> _vao_map;
    unsigned tvbo = 0;
};
