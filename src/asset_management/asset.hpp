#pragma once

#include "graphics/graphics_fwd.hpp"
#include "scripting/scripting_fwd.hpp"

#include "common/file.hpp"

namespace assets
{
struct asset
{
    using data_type = std::variant<std::shared_ptr<scripting::script>,
                                   std::shared_ptr<graphics::mesh>,
                                   std::shared_ptr<graphics::shader>,
                                   std::shared_ptr<graphics::material>,
                                   std::shared_ptr<graphics::font>,
                                   std::shared_ptr<texture>>;
    common::file _asset_file;
    data_type _data;

    template <typename T>
    std::shared_ptr<T> as() const
    {
        if (is_of_type<T>())
            return std::get<std::shared_ptr<T>>(_data);
        else
            return nullptr;
    }

    bool is_of_type(int type_index) const
    {
        return type_index == _data.index();
    }

    template <typename T>
    bool is_of_type() const
    {
        return std::holds_alternative<std::shared_ptr<T>>(_data);
    }
};
} // namespace assets
