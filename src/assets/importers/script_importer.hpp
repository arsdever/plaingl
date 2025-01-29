#pragma once

#include "graphics/graphics_fwd.hpp"
#include "scripting/scripting_fwd.hpp"

#include "assets/type_importer.hpp"

namespace assets
{
class script_importer : public type_importer<scripting::script>
{
public:
    static constexpr std::string_view extensions { ".py" };
    void internal_load(common::file& asset_file) override;
    void internal_update(asset_data_t scr, common::file& asset_file) override;
};
} // namespace assets
