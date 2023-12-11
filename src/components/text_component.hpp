#pragma once

#include "component.hpp"

class text_component : public component
{
public:
    text_component(game_object* parent);

    void set_text(std::string_view str);
    std::string_view get_text();

    static constexpr std::string_view class_type_id = "text_component";

private:
    std::string _text;
};
