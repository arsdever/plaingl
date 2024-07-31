#pragma once

namespace common
{
/**
 * @brief File change type
 */
enum class file_change_type
{
    created,
    removed,
    modified,
    renamed,
    unknown,
};

class file_watcher
{
public:
    file_watcher();
    file_watcher(
        std::string_view path,
        std::function<void(std::string_view, file_change_type)> functor);
    file_watcher& operator=(file_watcher&&);
    file_watcher(file_watcher&&);
    file_watcher(const file_watcher&) = delete;
    file_watcher& operator=(const file_watcher&) = delete;
    ~file_watcher();

private:
    struct impl;
    std::unique_ptr<impl> _impl;
};
} // namespace common
