#pragma once

#include <unordered_map>

#include "components/renderer_component.hpp"

struct GLFWwindow;
class font;
class material;

class text_renderer_component : public renderer_component
{
public:
    text_renderer_component(game_object* parent);

    void set_font(font* ttf);
    font* get_font();

    void init() override;
    void render() override;
    void draw_gizmos() override;
    void deinit() override;

    static constexpr std::string_view class_type_id = "text_renderer_component";

private:
    font* _font = nullptr;
    std::unordered_map<GLFWwindow*, unsigned int> _vao_map;
    unsigned tvbo = 0;
};
