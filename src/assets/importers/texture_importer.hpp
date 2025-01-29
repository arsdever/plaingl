#pragma once

#include "assets/type_importer.hpp"

namespace assets
{
class texture_importer : public assets::type_importer<graphics::texture>
{
public:
    static constexpr std::string_view extensions { ".png,.jpg,.jpeg" };
    void internal_load(common::file& asset_file) override;

    void internal_update(std::shared_ptr<graphics::texture>,
                         common::file&) override;
};
} // namespace assets
