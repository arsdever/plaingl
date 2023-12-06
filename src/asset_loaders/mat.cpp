#include <nlohmann/json.hpp>
#include <png.h>

#include "asset_loaders/mat.hpp"

#include "file.hpp"
#include "material.hpp"

using json = nlohmann::json;

void asset_loader_MAT::load(std::string_view path)
{
    std::string content = get_file_contents(path);
    json mat_struct = json(content);
    material* mat = new material;
}

material* asset_loader_MAT::get_material() { return _material; }
