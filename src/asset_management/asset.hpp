#pragma once

#include "common/file.hpp"

namespace graphics
{
class shader;
class shader_script;
class material;
} // namespace graphics
class mesh;
class texture;

namespace core
{
struct asset
{
    using data_type = std::variant<std::shared_ptr<mesh>,
                                   std::shared_ptr<graphics::shader>,
                                   std::shared_ptr<graphics::material>,
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
} // namespace core
