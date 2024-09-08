#include "asset_management/asset_manager.hpp"

#include "asset_management/asset.hpp"
#include "asset_management/asset_cache.hpp"
#include "asset_management/asset_importer.hpp"
#include "asset_management/importers/material_importer.hpp"
#include "asset_management/importers/model_importer.hpp"
#include "asset_management/importers/shader_importer.hpp"
#include "asset_management/importers/texture_importer.hpp"
#include "common/directory.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"

namespace assets
{
struct asset_manager::impl
{
    std::string project_path;
    common::file_watcher directory_watcher;

    asset_cache _cache;
    asset_importer _importer;
};

void asset_manager::initialize(asset_manager* existing_instance)
{
    if (_impl)
        return;

    _impl = existing_instance->_impl;
}

void asset_manager::initialize(std::string_view resource_path)
{
    _impl = std::make_shared<impl>();
    _impl->project_path = common::filesystem::path(resource_path).full_path();

    register_importer(texture_importer::extensions,
                      std::make_shared<texture_importer>());
    register_importer(material_importer::extensions,
                      std::make_shared<material_importer>());
    register_importer(shader_importer::extensions,
                      std::make_shared<shader_importer>());
    register_importer(model_importer::extensions,
                      std::make_shared<model_importer>());

    scan_directory();
    setup_directory_watch();
}

void asset_manager::shutdown()
{
    if (!_impl)
        return;

    _impl = nullptr;
}

asset& asset_manager::get(std::string_view name) { return *try_get(name); }

std::shared_ptr<asset> asset_manager::try_get_internal(std::string_view name,
                                                       int type_index)
{
    return _impl->_cache.find(name, type_index);
}

std::string_view asset_manager::internal_resource_path() { return ""; }

void asset_manager::scan_directory()
{
    auto project_path = common::filesystem::path(_impl->project_path);
    std::function<void(common::filesystem::path)> scan_dir;
    scan_dir = [ & ](common::filesystem::path path)
    {
        common::directory dir(std::string(path.full_path()));
        dir.visit_files(
            [ scan_dir, path ](std::string file_path, bool is_dir)
        {
            if (is_dir)
            {
                if (file_path == "." || file_path == "..")
                {
                    return;
                }

                scan_dir(path / file_path);
            }
            else
            {
                auto asset_path = path / file_path;
                _impl->_importer.import(asset_path.full_path(), _impl->_cache);
            }
        });
    };
    scan_dir(project_path);
}

void asset_manager::setup_directory_watch()
{
    _impl->directory_watcher = common::file_watcher(
        _impl->project_path,
        [](std::string_view path, common::file_change_type change)
    {
        switch (change)
        {
        case common::file_change_type::created:
            _impl->_importer.import(path, _impl->_cache);
            break;
        // case common::file_change_type::modified: reload_asset(path); break;
        // case common::file_change_type::removed: unload_asset(path); break;
        default: break;
        }
    });
}

void asset_manager::register_importer(
    std::string_view key, std::shared_ptr<type_importer_base> importer)
{
    _impl->_importer.register_importer(key, importer);
}

asset_importer& asset_manager::get_importer() { return _impl->_importer; }

asset_cache& asset_manager::get_cache() { return _impl->_cache; }

std::shared_ptr<asset_manager::impl> asset_manager::_impl = nullptr;
} // namespace assets
