#pragma once

#include "event.hpp"

class file
{
    enum class state
    {
        invalid,
        closed,
        open,
    };

public:
    file();
    file(const file&) = delete;
    file& operator=(const file&) = delete;
    file(file&& o);
    file& operator=(file&& o);
    ~file();

    void open(std::string_view path, std::string_view privileges);
    void close();
    size_t size() const;
    size_t read(char* buffer, size_t length);
    std::string read_all();

    static void watch(std::string_view path);
    static event<void(std::string_view)> changed_externally;

    static std::string read_all(std::string_view path);
    static bool exists(std::string_view path);

private:
    std::FILE* _file_descriptor = nullptr;
    state _state = state::invalid;
};

std::tuple<std::string, std::string, std::string>
parse_path(std::string_view path);
