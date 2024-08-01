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
    file_watcher(const file_watcher&);
    file_watcher& operator=(const file_watcher&);
    ~file_watcher();

    bool is_valid() const;

private:
    struct impl;
    std::shared_ptr<impl> _impl;
};
} // namespace common
