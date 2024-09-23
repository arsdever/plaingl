
#include "asset_management/importers/script_importer.hpp"

#include "common/file_lock.hpp"
#include "scripting/backend.hpp"
#include "scripting/script.hpp"

namespace assets
{
void script_importer::internal_load(common::file& asset_file)
{
    common::file_lock file_lock(asset_file);
    _data = scripting::backend::load_script(asset_file);
}

void script_importer::internal_update(asset_data_t scr,
                                      common::file& asset_file)
{
    scripting::backend::update_script(scr);
}
} // namespace assets
