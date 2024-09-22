#pragma once

#include "asset_management/asset_manager.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"

namespace assets
{
namespace
{
inline static logger log() { return get_logger("asset_manager"); }
} // namespace

class type_importer_base
{
public:
    virtual ~type_importer_base() = default;

    virtual void import(std::string_view path, asset_cache& cache) = 0;
};

template <typename T>
class type_importer : public type_importer_base
{
public:
    void import(std::string_view path, asset_cache& cache) override
    {
        common::filesystem::path p { path };
        if (cache.contains<T>(p.stem()))
        {
            log()->warn("Asset {} was already loaded", path);
            return;
        }
        common::file asset_file { std::string(path) };
        internal_load(asset_file);
        auto a = std::make_shared<asset>(std::move(asset_file), _data);
        asset_file.changed += [ this, wasset = std::weak_ptr(a) ](auto ct)
        {
            if (wasset.expired())
                return;

            auto a = wasset.lock();
            if (ct == common::file_change_type::modified)
            {
                internal_reload(a);
            }
        };
        cache.register_asset(p.stem(), a);
    }

protected:
    virtual void internal_load(common::file& asset_file) = 0;
    virtual void internal_reload(std::shared_ptr<asset> asset)
    {
        log()->info("Reloading is not implemented");
    }

protected:
    std::shared_ptr<T> _data;
};
} // namespace assets
