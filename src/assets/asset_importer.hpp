#pragma once

#include "assets/assets_fwd.hpp"

#include "common/utils.hpp"

namespace assets
{
class asset_importer
{
public:
    void load_asset(asset& ast);
    void update_asset(asset& ast);

    void register_importer(std::string_view key,
                           std::shared_ptr<type_importer_base> importer);

private:
    std::shared_ptr<type_importer_base> get_importer(std::string_view path);

private:
    std::unordered_map<std::string,
                       std::shared_ptr<type_importer_base>,
                       string_hash,
                       std::equal_to<>>
        _importers;
};
} // namespace assets
