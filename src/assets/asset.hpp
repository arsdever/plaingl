#pragma once

#include "assets/assets_fwd.hpp"

#include "common/event.hpp"

namespace assets
{
struct asset
{
public:
    asset(std::string file_path, std::function<void(asset&)> loader);

    std::string file_path() const;

    size_t id() const;

    std::string key() const;

    std::optional<std::any>& get_raw_data();

    template <typename T>
    std::shared_ptr<T> get_raw_data() const;

    bool is_of_type(size_t type_index) const;

    bool is_loaded() const;

    event<void()> _on_modified;

    template <typename T>
    std::shared_ptr<T> as() const;

    template <typename T>
    bool is_of_type() const;

private:
    /**
     * @brief The file path associated with the asset
     */
    std::string _file_path;

    /**
     * @brief The asset's internal data
     */
    std::optional<std::any> _data;

    /**
     * @brief The list of assets this asset depends on
     */
    std::vector<std::shared_ptr<asset>> _dependencies;

    /**
     * @brief Used to load the asset data
     */
    std::function<void(asset&)> _loader;
};

template <typename T>
std::shared_ptr<T> asset::get_raw_data() const
{
    return std::any_cast<std::shared_ptr<T>>(
        const_cast<asset&>(*this).get_raw_data().value());
}

template <typename T>
std::shared_ptr<T> asset::as() const
{
    if (!is_loaded())
    {
        _loader(const_cast<asset&>(*this));
    }

    if (is_of_type<std::shared_ptr<T>>())
        return get_raw_data<T>();
    else
        return nullptr;
}

template <typename T>
bool asset::is_of_type() const
{
    return is_loaded() && is_of_type(typeid(T).hash_code());
}
} // namespace assets
