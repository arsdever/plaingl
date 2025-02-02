#include "assets/asset_importer.hpp"

#include "assets/asset.hpp"
#include "assets/asset_cache.hpp"
#include "assets/type_importer.hpp"
#include "common/file.hpp"
#include "common/logging.hpp"

namespace assets
{
void asset_importer::load_asset(asset& ast)
{
    auto format =
        std::string(common::filesystem::path(ast.file_path()).extension());

    get_importer(format)->load_asset(ast);
}

void asset_importer::update_asset(asset& ast)
{
    auto format =
        std::string(common::filesystem::path(ast.file_path()).extension());

    get_importer(format)->update_asset(ast);
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
    common::filesystem::path p { path };

    auto it = _importers.find(p.extension());

    if (it == _importers.end())
    {
        log()->warn("No importer found for extension {}", p.extension());
        return std::make_shared<type_importer_base>();
    }

    return it->second;
}
} // namespace assets
