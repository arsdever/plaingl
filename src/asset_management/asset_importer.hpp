#pragma once

#include "common/utils.hpp"

namespace core
{
class type_importer_base;
template <typename T>
class type_importer;
class asset_cache;

class asset_importer
{
public:
    void import(std::string_view path, asset_cache& cache);

    void register_importer(std::string_view key,
                           std::shared_ptr<type_importer_base> importer);

private:
    std::unordered_map<std::string,
                       std::shared_ptr<type_importer_base>,
                       string_hash,
                       std::equal_to<>>
        _importers;
};
} // namespace core
