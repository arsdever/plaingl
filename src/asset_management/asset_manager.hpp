#pragma once

#include "asset_management/asset.hpp"
#include "asset_management/asset_importer.hpp"
#include "common/utils.hpp"

class mesh;
class image;
namespace graphics
{
class material;
class shader;
} // namespace graphics

namespace core
{
class asset_importer;
class asset;

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

    static asset& get(std::string_view name);
    template <typename T>
    static std::shared_ptr<T> get(std::string_view name)
    {
        return try_get<T>(name)->template as<T>();
    }
    static std::shared_ptr<asset> try_get(std::string_view name)
    {
        return try_get_internal(name, -1);
    }
    template <typename T>
    static std::shared_ptr<asset> try_get(std::string_view name)
    {
        auto type_index = variant_index_v<asset::data_type, std::shared_ptr<T>>;
        return try_get_internal(name, type_index);
    }

    static void register_importer(std::string_view key,
                                  std::shared_ptr<type_importer_base> importer);

    static asset_importer& get_importer();
    static asset_cache& get_cache();

private:
    static std::string_view internal_resource_path();

    static void scan_directory();
    static void setup_directory_watch();

    static std::shared_ptr<asset> try_get_internal(std::string_view name,
                                                   int type_index);

private:
    struct impl;
    static std::shared_ptr<impl> _impl;
};
} // namespace core
