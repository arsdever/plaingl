#pragma once

#include "asset_management/type_importer.hpp"

class model_importer : public core::type_importer<mesh>
{
public:
    static constexpr std::string_view extensions { ".fbx" };
    void internal_load(common::file& asset_file) override;
};
