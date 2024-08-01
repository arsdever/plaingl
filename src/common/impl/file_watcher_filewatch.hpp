#pragma once

#include <FileWatch.hpp>

#include "common/file_watcher.hpp"

namespace common
{
struct file_watcher::impl
{
    filewatch::FileWatch<std::string> watch;

    static file_change_type get_change_type(filewatch::Event e)
    {
        switch (e)
        {
        case filewatch::Event::added: return file_change_type::created;
        case filewatch::Event::modified: return file_change_type::modified;
        case filewatch::Event::removed: return file_change_type::removed;
        case filewatch::Event::renamed_new:
        case filewatch::Event::renamed_old: return file_change_type::renamed;
        default: return file_change_type::unknown;
        }
        return file_change_type::unknown;
    };

    static impl
    create(std::string_view path,
           std::function<void(std::string_view, file_change_type)> functor)
    {
        return { filewatch::FileWatch<std::string>(
            std::string(path),
            [ functor ](const std::string& path,
                        const filewatch::Event change_type)
        { functor(path, get_change_type(change_type)); }) };
    }
};
} // namespace common
