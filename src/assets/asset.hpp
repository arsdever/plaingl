#pragma once

#include "graphics/graphics_fwd.hpp"
#include "scripting/scripting_fwd.hpp"

#include "assets/asset_manager.hpp"
#include "common/event.hpp"
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
                                   std::shared_ptr<graphics::texture>,
                                   std::shared_ptr<void>>;
    common::file _asset_file;
    std::optional<data_type> _data;
    std::vector<std::shared_ptr<asset>> _dependencies;

    event<void()> _on_modified;

    template <typename T>
    std::shared_ptr<T> as() const
    {
        if (!is_loaded())
        {
            asset_manager::load_asset(*const_cast<asset*>(this));
        }

        if (is_of_type<T>())
            return std::get<std::shared_ptr<T>>(*_data);
        else
            return nullptr;
    }

    bool is_of_type(int type_index) const
    {
        return type_index == _data->index();
    }

    template <typename T>
    bool is_of_type() const
    {
        return is_loaded() &&
               std::holds_alternative<std::shared_ptr<T>>(*_data);
    }

    bool is_loaded() const { return _data.has_value(); }
};
} // namespace assets
