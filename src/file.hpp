#pragma once

#include <string>
#include <tuple>

std::string get_file_contents(std::string_view path);

bool file_exists(std::string_view path);

std::tuple<std::string, std::string, std::string>
parse_path(std::string_view path);
