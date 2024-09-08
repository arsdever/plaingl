#pragma once

#include "asset_management/asset.hpp"
#include "common/utils.hpp"

namespace assets
{
class asset;

class asset_cache
{
public:
    void register_asset(std::string_view name, std::shared_ptr<asset> ast);

    std::shared_ptr<asset> find(std::string_view name, int type_index = -1);
    bool contains(std::string_view name, int type_index = -1);

    template <typename T>
    bool contains(std::string_view name)
    {
        return contains(name, variant_index_v<asset::data_type, T>);
    }

    template <typename T>
    std::shared_ptr<asset> find(std::string_view name)
    {
        return find(name, variant_index_v<asset::data_type, T>);
    }

private:
    std::unordered_multimap<std::string,
                            std::shared_ptr<asset>,
                            string_hash,
                            std::equal_to<>>
        _assets;
};
} // namespace assets
