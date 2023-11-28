#pragma once

#include <string>

class file
{
    enum class state
    {
        closed,
        open,
    };

public:
    file();
    ~file();

    void open(std::string_view path, std::string_view privileges);
    void close();
	size_t size() const;
	size_t read(char* buffer, size_t length);
	std::string read_all();

private:
    std::FILE* _file_descriptor;
	state _state;
};

std::string get_file_contents(std::string_view path);
