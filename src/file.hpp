#pragma once

#include "event.hpp"

class file
{
private:
    enum class state
    {
        invalid,
        closed,
        open_read,
        open_write,
    };

    struct private_data;

public:
    enum class event_type
    {
        added,
        removed,
        modified,
        renamed,
        unknown,
    };

    class file_watch_hook
    {
    private:
        struct pdata;

    public:
        file_watch_hook(pdata&&);
        file_watch_hook& operator=(file_watch_hook&&);
        file_watch_hook(file_watch_hook&&);
        file_watch_hook() = delete;
        file_watch_hook(const file_watch_hook&) = delete;
        file_watch_hook& operator=(const file_watch_hook&) = delete;
        ~file_watch_hook();

    private:
        std::unique_ptr<pdata> _p;
    };

public:
    file(std::string path);
    file(const file&) = delete;
    file& operator=(const file&) = delete;
    file(file&& o) = default;
    file& operator=(file&& o) = default;
    ~file();

    void open(std::string_view mode) const;
    void close() const;
    size_t size() const;
    bool exists() const;
    void watch();

    size_t read(char* buffer, size_t length) const;
    std::string read_all() const;

    event<void(event_type)> changed_externally;

    static file_watch_hook
    watch(std::string_view path,
          std::function<void(std::string_view, event_type)> functor);
    static event<void(std::string_view)> generic_file_change;

    static std::string read_all(std::string_view path);
    static bool exists(std::string_view path);
    static std::tuple<std::string, std::string, std::string>
    parse_path(std::string_view path);

private:
    std::string read_all_open() const;

private:
    std::unique_ptr<private_data> _pdata;
};
