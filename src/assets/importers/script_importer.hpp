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
    void initialize_asset(asset& ast) override;
    void read_asset_data(std::string_view asset_path) override;
};
} // namespace assets
