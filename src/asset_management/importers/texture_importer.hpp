#pragma once

#include "asset_management/type_importer.hpp"

class texture_importer : public core::type_importer<texture>
{
public:
    static constexpr std::string_view extensions { ".png,.jpg,.jpeg" };
    void internal_load(common::file& asset_file) override;
};
