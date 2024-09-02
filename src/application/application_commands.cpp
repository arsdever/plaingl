#include "application/application_commands.hpp"

#include "common/logging.hpp"
#include "core/asset_manager.hpp"
#include "graphics/texture.hpp"
#include "tools/mesh_viewer/mesh_viewer.hpp"
#include "tools/profiler/profiler.hpp"
#include "tools/texture_viewer/texture_viewer.hpp"

namespace
{
logger log() { return get_logger("console"); }
} // namespace

void cmd_show_profiler::execute()
{
    auto pv = std::make_shared<profiler>();
    pv->init();
    open_window_requested(pv);
}

void cmd_show_mesh::execute()
{
    int num = get<0>();
    if (num < 0 || num >= core::asset_manager::meshes().size())
    {
        log()->error("Mesh {} not found", num);
        return;
    }

    auto mv = std::make_shared<mesh_viewer>();
    mv->init();
    log()->debug("Showing mesh {}", num);
    mv->set_mesh(core::asset_manager::meshes()[ num ]);
    open_window_requested(mv);
}

void cmd_show_texture::execute()
{
    int num = get<0>();
    if (num < 0 || num >= core::asset_manager::textures().size())
    {
        log()->error("Texture {} not found", num);
        return;
    }

    auto tv = std::make_shared<texture_viewer>();
    tv->init();
    log()->debug("Showing texture {}", num);
    tv->set_texture(texture::_textures[ num ]);
    open_window_requested(tv);
}

void cmd_list_textures::execute()
{
    for (int i = 0; i < texture::_textures.size(); ++i)
    {
        log()->info(
            "Texture {}: id {}", i, texture::_textures[ i ]->native_id());
    }
}

event<void(std::shared_ptr<core::window>)> cmd_show_mesh::open_window_requested;
event<void(std::shared_ptr<core::window>)>
    cmd_show_profiler::open_window_requested;
event<void(std::shared_ptr<core::window>)>
    cmd_show_texture::open_window_requested;
