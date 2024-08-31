#pragma once

struct shader_property
{
    std::string name;
    int location_info;
    int size;
    std::any value;
};
