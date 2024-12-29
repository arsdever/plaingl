#pragma once

#include "graphics/graphics_fwd.hpp"

#include "asset_management/type_importer.hpp"

class model_importer : public assets::type_importer<graphics::mesh>
{
public:
    static constexpr std::string_view extensions { ".fbx" };
    void internal_load(common::file& asset_file) override;
    void internal_update(std::shared_ptr<mesh> m,
                         common::file& asset_file) override;
};
