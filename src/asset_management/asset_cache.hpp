#pragma once

#include "asset_management/assets_fwd.hpp"

#include "asset_management/asset.hpp"
#include "common/utils.hpp"

namespace assets
{
class asset_cache
{
public:
    void register_asset(std::string_view name, std::shared_ptr<asset> ast);
    void register_asset(size_t id, std::shared_ptr<asset> ast);

    bool contains(std::string_view name);
    bool contains(size_t id);

    std::shared_ptr<asset> find(size_t i);
    std::shared_ptr<asset> find(std::string_view name);

private:
    std::unordered_map<size_t, std::shared_ptr<asset>> _assets;
};
} // namespace assets
