#pragma once

namespace core
{
class object;

class asset_importer_context
{
public:
    std::string _path;
    std::vector<std::shared_ptr<object>> _objects;
};
} // namespace core
