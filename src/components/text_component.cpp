#include "components/text_component.hpp"

text_component::text_component(game_object* parent)
    : component(parent, class_type_id)
{
}

void text_component::set_text(std::string_view str) { _text = str; }

std::string_view text_component::get_text() { return _text; }
