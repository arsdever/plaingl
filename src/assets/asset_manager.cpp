#include <nlohmann/json.hpp>

#include "assets/asset_manager.hpp"

#include "assets/asset.hpp"
#include "assets/asset_cache.hpp"
#include "assets/asset_importer.hpp"
#include "assets/importers/font_importer.hpp"
#include "assets/importers/material_importer.hpp"
#include "assets/importers/model_importer.hpp"
#include "assets/importers/script_importer.hpp"
#include "assets/importers/shader_importer.hpp"
#include "assets/importers/texture_importer.hpp"
#include "common/directory.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "common/main_thread_dispatcher.hpp"

namespace assets
{
struct asset_manager::impl
{
    std::string project_path;
    common::file_watcher directory_watcher;

    asset_cache _cache;
    asset_importer _importer;
    std::queue<std::string> _loading_queue;
    std::unordered_set<std::string> _visited_list;
    std::stack<std::string> _visit_list;
    nlohmann::json _index { { "id_path", nlohmann::json {} },
                            { "key_id", nlohmann::json {} },
                            { "requires", nlohmann::json {} },
                            { "no_lazy_load", nlohmann::json {} } };

    void scan_directory()
    {
        load_index_file();
        update_index();
        save_index();

        resolve_dependencies();
        resolve_no_lazy_load();
    }

    void load_index_file()
    {
        auto project_path = common::filesystem::path(_impl->project_path);
        common::file index_file(
            std::string((project_path / "resources.json").full_path()));

        if (index_file.exists())
        {
            auto data = index_file.read_all();
            _index = nlohmann::json::parse(data);
            index_file.close();
        }
    }

    void update_index()
    {
        auto project_path = common::filesystem::path(_impl->project_path);
        common::file index_file(
            std::string((project_path / "resources.json").full_path()));

        std::function<void(common::filesystem::path)> scan_dir;
        scan_dir = [ & ](common::filesystem::path path)
        {
            common::directory dir(std::string(path.full_path()));
            dir.visit_files(
                [ this, scan_dir, path ](std::string file_path, bool is_dir)
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
                    auto asset_key = get_asset_key(asset_path.full_path());

                    if (!_index[ "key_id" ].contains(asset_key))
                    {
                        auto id = generate_random<size_t>();
                        _index[ "key_id" ][ asset_key ] = std::to_string(id);
                        _index[ "id_path" ][ std::to_string(id) ] =
                            path.full_path();
                    }

                    auto ast = std::make_shared<asset>(
                        common::file(std::string(asset_path.full_path())));
                    _cache.register_asset(
                        std::stoull(
                            _index[ "key_id" ][ asset_key ].get<std::string>()),
                        std::move(ast));
                }
            });
        };
        scan_dir(project_path);
    }

    void save_index()
    {
        auto project_path = common::filesystem::path(_impl->project_path);
        common::file index_file(
            std::string((project_path / "resources.json").full_path()));

        // if (!index_file.exists())
        // {
        common::file::remove(index_file.get_filepath());
        common::file::create(index_file.get_filepath(), _index.dump());
        // }
        // else
        // {
        //     index_file.open(common::file::open_mode::write);
        //     index_file.write(_index.dump());
        // }
    }

    void schedule_loading_queue()
    {
        while (!_visit_list.empty())
        {
            auto id = _visit_list.top();
            if (_visited_list.contains(id))
            {
                _visit_list.pop();
                _loading_queue.push(id);
                continue;
            }

            if (!_index[ "requires" ].contains(id))
            {
                _visit_list.pop();
                _visited_list.emplace(id);
                continue;
            }

            for (const auto& deps : _index[ "requires" ][ id ])
            {
                _visit_list.push(deps);
                _visited_list.emplace(id);
            }
        }
    }

    std::string get_resource_path_by_id(std::string id)
    {
        return _index[ "id_path" ][ id ];
    }

    void load_assets()
    {
        while (!_loading_queue.empty())
        {
            auto id = _loading_queue.front();
            _loading_queue.pop();
            auto path = get_resource_path_by_id(id);
            _importer.import(path, _impl->_cache);

            if (!_index[ "requires" ].contains(id))
            {
                continue;
            }

            for (const auto& dep : _index[ "requires" ][ id ])
            {
                auto dep_path = _index[ "id_path" ][ dep ];
                auto ast = _impl->_cache.find(get_asset_key(dep_path));
                if (!ast)
                {
                    log()->warn(
                        "The dependency asset {} of asset {} was not loaded",
                        dep.get<std::string>(),
                        id);
                    continue;
                }
                ast->_on_modified += [ this, id ]()
                {
                    auto path = _index[ "id_path" ][ id ];
                    _cache.register_asset(
                        id, std::make_shared<asset>(path.get<std::string>()));
                };
            }
        }
    }

    void resolve_dependencies()
    {
        for (const auto& [ base, deps ] : _index[ "requires" ].items())
        {
            auto base_ast = _impl->_cache.find(std::stoull(base));
            if (!base_ast)
            {
                continue;
            }

            for (const auto& dep : deps)
            {
                auto ast =
                    _impl->_cache.find(std::stoull(dep.get<std::string>()));
                base_ast->_dependencies.push_back(ast);
                if (ast)
                {
                    ast->_on_modified += [ this, base ]()
                    {
                        auto path = _index[ "id_path" ][ base ];
                        _importer.update(path, _impl->_cache);
                    };
                }
            }
        }
    }

    void resolve_no_lazy_load()
    {
        for (const auto& it : _index[ "no_lazy_load" ])
        {
            auto ast = _impl->_cache.find(std::stoull(it.get<std::string>()));
            if (!ast)
            {
                log()->warn("Invalid entry in no_lazy_load list: {}",
                            it.get<std::string>());
                continue;
            }

            load_asset(*ast);
        }
    }

    void load_asset(std::string_view path)
    {
        auto asset_key = get_asset_key(path);
        log()->debug("Loading asset {}", asset_key);

        if (!_index[ "key_id" ].contains(asset_key))
        {
            log()->warn("Asset {} is not in the index", asset_key);
            return;
        }

        if (_cache.contains(asset_key))
        {
            _importer.update(path, _cache);
            return;
        }

        _importer.import(path, _cache);
    }

    void setup_directory_watch()
    {
        try
        {
            directory_watcher = common::file_watcher(
                project_path,
                [ this ](std::string_view path, common::file_change_type change)
            {
                std::string path_str { (common::filesystem::path(
                                            _impl->project_path) /
                                        common::filesystem::path(path))
                                           .full_path() };
                common::main_thread_dispatcher::dispatch(
                    [ this, path = std::move(path_str), change ]
                {
                    switch (change)
                    {
                    case common::file_change_type::created:
                    case common::file_change_type::modified:
                    {
                        auto ast = _impl->_cache.find(get_asset_key(path));
                        if (ast)
                        {
                            load_asset(*ast);
                        }
                    }
                    // case common::file_change_type::removed:
                    // unload_asset(path); break;
                    default: break;
                    }
                });
            });
        }
        catch (std::exception& e)
        {
            log()->error("Failed to setup directory watcher: {}", e.what());
            return;
        }
    }

    std::shared_ptr<asset> try_get_internal(std::string_view name)
    {
        return _impl->_cache.find(name);
    }

    std::string_view internal_resource_path() { return ""; }

    void load_asset(asset& ast)
    {
        _importer.load_asset(ast);
        ast._on_modified();
    }
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

    register_importer(font_importer::extensions,
                      std::make_shared<font_importer>());
    register_importer(texture_importer::extensions,
                      std::make_shared<texture_importer>());
    register_importer(material_importer::extensions,
                      std::make_shared<material_importer>());
    register_importer(script_importer::extensions,
                      std::make_shared<script_importer>());
    register_importer(shader_importer::extensions,
                      std::make_shared<shader_importer>());
    register_importer(model_importer::extensions,
                      std::make_shared<model_importer>());

    _impl->scan_directory();
    _impl->setup_directory_watch();
}

void asset_manager::shutdown()
{
    if (!_impl)
        return;

    _impl = nullptr;
}

void asset_manager::load_asset(asset& ast) { _impl->load_asset(ast); }

asset& asset_manager::get(std::string_view name) { return *try_get(name); }

std::shared_ptr<asset> asset_manager::try_get_internal(std::string_view name)
{
    return _impl->try_get_internal(name);
}

std::string asset_manager::get_asset_key(std::string_view path)
{
    auto pos = path.find(_impl->project_path);
    auto relpath = path.substr(
        pos != std::string::npos ? pos + _impl->project_path.size() + 1 : 0);
    std::string result = std::string(relpath);
    std::replace(result.begin(), result.end(), '/', '.');
    return std::string(result);
}

void asset_manager::register_importer(
    std::string_view key, std::shared_ptr<type_importer_base> importer)
{
    _impl->_importer.register_importer(key, importer);
}

asset_importer& asset_manager::get_importer() { return _impl->_importer; }

asset_cache& asset_manager::get_cache() { return _impl->_cache; }

size_t asset_manager::get_asset_id(std::string_view path)
{
    if (!_impl->_index[ "key_id" ].contains(path))
    {
        return 0;
    }
    return std::stoull(_impl->_index[ "key_id" ][ path ].get<std::string>());
}

std::shared_ptr<asset_manager::impl> asset_manager::_impl = nullptr;
} // namespace assets
