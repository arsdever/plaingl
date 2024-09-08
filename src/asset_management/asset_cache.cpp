#include "asset_management/asset_cache.hpp"

namespace assets
{
std::shared_ptr<asset> asset_cache::find(std::string_view name, int type_index)
{
    if (type_index == -1)
    {
        auto it = _assets.find(std::string(name));
        if (it == _assets.end())
        {
            return nullptr;
        }
        return it->second;
    }
    else
    {
        auto index = _assets.bucket(std::string(name));
        if (_assets.bucket_size(index) == 0)
        {
            return nullptr;
        }

        for (auto it = _assets.begin(index); it != _assets.end(index); ++it)
        {
            auto ast = it->second;
            if (ast->is_of_type(type_index))
            {
                return ast;
            }
        }

        return nullptr;
    }
}

bool asset_cache::contains(std::string_view name, int type_index)
{
    return find(name, type_index) != nullptr;
}

void asset_cache::register_asset(std::string_view name,
                                 std::shared_ptr<asset> ast)
{
    _assets.emplace(name, ast);
}
} // namespace assets
