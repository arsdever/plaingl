#include "asset_management/asset_importer.hpp"

#include "asset_management/asset.hpp"
#include "asset_management/asset_cache.hpp"
#include "asset_management/type_importer.hpp"
#include "common/file.hpp"
#include "common/logging.hpp"

namespace assets
{
void asset_importer::import(std::string_view path, asset_cache& cache)
{
    get_importer(path)->import(path, cache);
}

void asset_importer::update(std::string_view path, asset_cache& cache)
{
    get_importer(path)->update(path, cache);
}

void asset_importer::load_asset(asset& ast)
{
    auto format = std::string(
        common::filesystem::path(ast._asset_file.get_filepath()).extension());

    get_importer(format)->load_asset(ast);
}

void asset_importer::register_importer(
    std::string_view key, std::shared_ptr<type_importer_base> importer)
{
    std::string_view::size_type ext_begin = 0; // Must initialize
    std::string_view::size_type pos = 0;       // Must initialize
    do
    {
        pos = key.find(',', ext_begin);
        auto ext = key.substr(ext_begin, pos - ext_begin);
        _importers[ std::string(ext) ] = importer;
        ext_begin = pos + 1;
    } while (pos != std::string_view::npos);
}

std::shared_ptr<type_importer_base>
asset_importer::get_importer(std::string_view path)
{
    struct empty_importer : public type_importer_base
    {
        void import(std::string_view path, asset_cache& cache) override
        {
            auto ast = std::make_shared<asset>(common::file(std::string(path)));
            cache.register_asset(path, ast);
        }
        void update(std::string_view path, asset_cache& cache) override { }
    };

    common::filesystem::path p { path };

    auto it = _importers.find(p.extension());

    if (it == _importers.end())
    {
        log()->warn("No importer found for extension {}", p.extension());
        return std::make_shared<empty_importer>();
    }

    return it->second;
}
} // namespace assets
