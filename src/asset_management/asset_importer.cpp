#include "asset_management/asset_importer.hpp"

#include "asset_management/type_importer.hpp"
#include "common/logging.hpp"

namespace assets
{
void asset_importer::import(std::string_view path, asset_cache& cache)
{
    common::filesystem::path p { path };

    auto it = _importers.find(p.extension());

    if (it == _importers.end())
    {
        log()->warn("No importer found for extension {}", p.extension());
        return;
    }

    it->second->import(path, cache);
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
} // namespace assets
