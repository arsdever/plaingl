#pragma once

#include "scripting/scripting_fwd.hpp"

#include "asset_management/type_importer.hpp"

namespace assets
{
class script_importer : public type_importer<scripting::script>
{
public:
    static constexpr std::string_view extensions { ".py" };
    void internal_load(common::file& asset_file) override;
};
} // namespace assets
