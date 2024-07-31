#pragma once

#include <Windows.h>

#include "common/file.hpp"
#include "common/logging.hpp"

namespace common
{
namespace
{
inline logger log() { return get_logger("file"); }
} // namespace

struct file::impl
{
    std::string path {};
    HANDLE file { nullptr };

    bool is_open() { return file != nullptr; }

    void open(file::open_mode mode)
    {
        size_t win_mode = 0;
        switch (mode)
        {
        case file::open_mode::read: win_mode = GENERIC_READ; break;
        case file::open_mode::write: win_mode = GENERIC_WRITE; break;
        case file::open_mode::read_write:
            win_mode = GENERIC_READ | GENERIC_WRITE;
            break;
        case file::open_mode::append: win_mode = FILE_APPEND_DATA; break;
        default: break;
        }

        file = CreateFile(path.data(),
                          win_mode,
                          0,
                          nullptr,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          nullptr);

        if (file == INVALID_HANDLE_VALUE || file == nullptr)
        {
            // get error message
            DWORD error = GetLastError();
            log()->error("Error opening file {}: {:#010x}", path, error);
            file = nullptr;
        }
    }

    void close()
    {
        if (file == nullptr)
            return;

        CloseHandle(file);
        file = nullptr;
    }

    size_t get_content_size()
    {
        if (file == nullptr)
            return 0;

        LARGE_INTEGER size;
        size.LowPart =
            GetFileSize(file, reinterpret_cast<DWORD*>(&size.HighPart));
        return static_cast<size_t>(size.QuadPart);
    }

    size_t read(void* buffer, size_t size)
    {
        if (file == nullptr)
            return 0;

        DWORD bytes_read { 0 };
        if (!ReadFile(
                file, buffer, static_cast<DWORD>(size), &bytes_read, nullptr))
        {
            DWORD error = GetLastError();
            log()->error("Error reading file {}: {}", path, error);
        }

        return bytes_read;
    }

    void seek(size_t position)
    {
        if (file == nullptr)
            return;

        LARGE_INTEGER pos;
        pos.QuadPart = position;
        SetFilePointer(file, pos.LowPart, &pos.HighPart, FILE_BEGIN);
    }

    static impl create(std::string_view path, std::string_view contents)
    {
        HANDLE file = CreateFile(path.data(),
                                 GENERIC_READ | GENERIC_WRITE,
                                 0,
                                 nullptr,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 nullptr);

        if (file == INVALID_HANDLE_VALUE)
        {
            return {};
        }

        if (contents.size() > 0)
        {
            DWORD bytes_written;
            WriteFile(file,
                      contents.data(),
                      static_cast<DWORD>(contents.size()),
                      &bytes_written,
                      nullptr);
        }

        return { std::string(path), file };
    }

    static std::string current_dir()
    {
        std::string path;
        path.resize(MAX_PATH);
        auto sz = GetCurrentDirectory(MAX_PATH, path.data());
        path.resize(sz);
        return path;
    }

    static bool exists(std::string_view path)
    {
        DWORD dwAttrib = GetFileAttributes(path.data());

        bool result = (dwAttrib != INVALID_FILE_ATTRIBUTES &&
                       !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

        if (!result)
        {
            log()->trace(
                "No file {} exists in directory {}", path, current_dir());
        }

        return result;
    }
};
} // namespace common
