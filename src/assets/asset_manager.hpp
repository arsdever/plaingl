#pragma once

#include "assets/assets_fwd.hpp"
#include "graphics/graphics_fwd.hpp"

#include "assets/asset.hpp"
#include "assets/asset_importer.hpp"
#include "common/utils.hpp"

namespace assets
{
class asset_manager
{
private:
    asset_manager() = default;

public:
    static void initialize(asset_manager* existing_instance);
    static void initialize(std::string_view resource_path);
    static void shutdown();

    template <typename T>
    static void save_asset(std::string_view path, const T* asset);
    static void register_asset(std::string_view name,
                               std::shared_ptr<asset> ast);

    static void load_asset(asset& ast);

    static asset& get(std::string_view name);
    template <typename T>
    static auto get(std::string_view name)
    {
        return try_get<T>(name);
    }
    static auto try_get(std::string_view name)
    {
        return try_get_internal(name);
    }
    template <typename T>
    static std::shared_ptr<T> try_get(std::string_view name)
    {
        auto type_index = variant_index_v<asset::data_type, std::shared_ptr<T>>;
        auto ast = try_get_internal(name);
        if (!ast)
            return nullptr;

        return ast->template as<T>();
    }

    static void register_importer(std::string_view key,
                                  std::shared_ptr<type_importer_base> importer);

    static asset_importer& get_importer();
    static asset_cache& get_cache();

    static size_t get_asset_id(std::string_view path);
    static std::string get_asset_key(std::string_view path);

private:
    static std::shared_ptr<asset> try_get_internal(std::string_view name);

private:
    struct impl;
    static std::shared_ptr<impl> _impl;
};
} // namespace assets
