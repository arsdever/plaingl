#pragma once

#include "assets/asset_manager.hpp"
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
    virtual void update(std::string_view path, asset_cache& cache) = 0;

    virtual void load_asset(asset& ast) { }
};

template <typename T>
class type_importer : public type_importer_base
{
public:
    using asset_data_t = std::shared_ptr<T>;

    void import(std::string_view path, asset_cache& cache) override
    {
        common::filesystem::path p { path };
        if (cache.contains(asset_manager::get_asset_id_by_path(p.full_path())))
        {
            log()->warn("Asset {} was already loaded", path);
            return;
        }
        common::file asset_file { std::string(path) };
        internal_load(asset_file);
        auto a = std::make_shared<asset>(std::move(asset_file), _data);
        // a._date_modified =
        // std::chrono::system_clock::now().time_since_epoch().count();
        cache.register_asset(asset_manager::get_asset_id_by_path(p.full_path()),
                             a);
    }

    void update(std::string_view path, asset_cache& cache) override
    {
        common::filesystem::path p { path };
        if (!cache.contains(asset_manager::get_asset_id_by_path(p.full_path())))
        {
            log()->info("Asset {} was not loaded", path);
            import(path, cache);
            return;
        }

        auto ast =
            cache.find(asset_manager::get_asset_id_by_path(p.full_path()));
        internal_update(ast->template as<T>(), ast->_asset_file);
    }

    void load_asset(asset& ast) override
    {
        internal_load(ast._asset_file);
        ast._data = _data;
    }

protected:
    virtual void internal_load(common::file& asset_file) = 0;
    virtual void internal_update(std::shared_ptr<T>, common::file&)
    {
        log()->info("Updating is not implemented");
    }

protected:
    std::shared_ptr<T> _data;
};
} // namespace assets
