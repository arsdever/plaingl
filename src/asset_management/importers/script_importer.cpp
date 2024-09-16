#include "asset_management/importers/script_importer.hpp"

#include "scripting/backend.hpp"
#include "scripting/script.hpp"

namespace assets
{
void script_importer::internal_load(common::file& asset_file)
{
    _data = scripting::backend::load_script(asset_file);
}
} // namespace assets
