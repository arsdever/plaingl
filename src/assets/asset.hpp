#pragma once

#include "assets/asset_manager.hpp"
#include "common/event.hpp"
#include "common/file.hpp"

namespace assets
{
struct asset
{
    using data_type = std::any;

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

        if (is_of_type<std::shared_ptr<T>>())
            return std::any_cast<std::shared_ptr<T>>(*_data);
        else
            return nullptr;
    }

    bool is_of_type(size_t type_index) const
    {
        return type_index == _data->type().hash_code();
    }

    template <typename T>
    bool is_of_type() const
    {
        return is_loaded() && is_of_type(typeid(T).hash_code());
    }

    bool is_loaded() const { return _data.has_value(); }
};
} // namespace assets
