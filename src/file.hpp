#pragma once

#include <string>

std::string get_file_contents(std::string_view path);

bool file_exists(std::string_view path);
