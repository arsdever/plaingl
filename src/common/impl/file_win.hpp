#pragma once

#include <Windows.h>

#include "common/directory.hpp"
#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"

namespace common
{
namespace
{
inline logger log() { return get_logger("file"); }
} // namespace

struct windows_file
{
    std::string path {};
    HANDLE file { nullptr };

    windows_file(std::string path)
        : path(std::move(path))
    {
    }

    bool is_open() { return file != nullptr; }

    bool is_directory()
    {
        DWORD attrib = GetFileAttributes(path.data());
        return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    void open(file::open_mode mode, uint32_t share_flag, uint32_t flags)
    {
        size_t win_mode = 0;
        size_t create_mode = OPEN_ALWAYS;
        switch (mode)
        {
        case file::open_mode::read:
            win_mode = GENERIC_READ;
            create_mode = OPEN_EXISTING;
            break;
        case file::open_mode::write: win_mode = GENERIC_WRITE; break;
        case file::open_mode::read_write:
            win_mode = GENERIC_READ | GENERIC_WRITE;
            break;
        case file::open_mode::append: win_mode = FILE_APPEND_DATA; break;
        default: break;
        }

        file = CreateFile(path.data(),
                          win_mode,
                          share_flag,
                          nullptr,
                          create_mode,
                          flags,
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

    void remove()
    {
        close();

        remove(path);
    }

    static void remove(std::string_view path)
    {
        if (!DeleteFile(path.data()))
        {
            DWORD error = GetLastError();
            log()->error("Error removing file {}: {}", path, error);
        }
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

    size_t write(const void* buffer, size_t size)
    {
        if (file == nullptr)
            return 0;

        DWORD bytes_written { 0 };
        if (!WriteFile(file,
                       buffer,
                       static_cast<DWORD>(size),
                       &bytes_written,
                       nullptr))
        {
            DWORD error = GetLastError();
            log()->error("Error writing to file {}: {}", path, error);
        }

        return bytes_written;
    }

    void seek(size_t position)
    {
        if (file == nullptr)
            return;

        LARGE_INTEGER pos;
        pos.QuadPart = position;
        SetFilePointer(file, pos.LowPart, &pos.HighPart, FILE_BEGIN);
    }
};

struct file::impl
{
    windows_file f;

    impl(std::string path)
        : f(std::move(path))
    {
    }

    std::string_view path() { return f.path; }

    void open(file::open_mode mode) { f.open(mode, 0, FILE_ATTRIBUTE_NORMAL); }

    bool is_open() { return f.is_open(); }

    bool is_directory() { return f.is_directory(); }

    size_t get_content_size() { return f.get_content_size(); }

    size_t read(void* buffer, size_t size) { return f.read(buffer, size); }

    size_t write(const void* buffer, size_t size)
    {
        return f.write(buffer, size);
    }

    void remove() { f.remove(); }

    static void remove(std::string_view path) { windows_file::remove(path); }

    void close() { f.close(); }

    void seek(size_t position) { f.seek(position); }

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
            return { "" };
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

        return { std::string(path) };
    }

    static bool exists(std::string_view path)
    {
        DWORD dwAttrib = GetFileAttributes(path.data());

        bool result = (dwAttrib != INVALID_FILE_ATTRIBUTES &&
                       !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

        if (!result)
        {
            log()->trace("No file {} exists in directory {}",
                         path,
                         filesystem::path::current_dir().full_path());
        }

        return result;
    }
};

struct directory::impl
{
    windows_file f;

    impl(std::string path)
        : f(std::move(path))
    {
    }

    ~impl() { close(); }

    void open(file::open_mode mode, uint32_t flags = FILE_FLAG_BACKUP_SEMANTICS)
    {
        f.open(mode, FILE_SHARE_DELETE, flags);
    }

    void close() { f.close(); }

    template <typename T = void>
    T traverse(std::function<T(std::string, bool)> callback)
    {
        log()->debug("Traversing {}", f.path);

        log()->trace("Looking for the first file in the directory {}", f.path);
        std::string lookup = f.path + "\\*";
        WIN32_FIND_DATAA data;
        HANDLE find_handle = FindFirstFile(lookup.c_str(), &data);

        if (find_handle == INVALID_HANDLE_VALUE)
        {
            log()->error("Failed to traverse the directory {}", f.path);
            return;
        }

        do
        {
            bool is_dir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            if constexpr (std::is_same_v<T, void>)
            {
                callback(std::string(data.cFileName), is_dir);
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                if (!callback(std::string(data.cFileName), is_dir))
                {
                    return false;
                }
            }
            else
            {
                static_assert(false && "T must be void or bool");
            }
        } while (FindNextFile(find_handle, &data));

        FindClose(find_handle);
    }
};

struct filesystem_impl
{
    static std::string convert_to_absolute_path(std::string_view path)
    {
        std::string result;
        static constexpr DWORD MAX_PATH_LENGTH = 4096;
        result.resize(MAX_PATH_LENGTH);
        auto sz = GetFullPathNameA(
            path.data(), MAX_PATH_LENGTH, result.data(), nullptr);
        result.resize(sz);
        static constexpr std::array<std::string_view, 3> slashes = { "\\\\",
                                                                     "\\",
                                                                     "\\/" };

        auto current = result.begin();
        auto replace = result.begin();
        while (replace != result.end())
        {
            while (*current != '\\')
            {
                *current = *replace;
                ++current;
                ++replace;

                if (replace == result.end())
                    break;
            }

            std::string_view viewer(current, result.end());

            for (auto s : slashes)
            {
                if (viewer.starts_with(s))
                {
                    *current = '/';
                    replace = current + s.size() - 1;
                    break;
                }
            }

            if (replace == result.end())
                break;

            ++current;
            ++replace;
        }
        result.erase(current, result.end());
        return result;
    }

    static std::string current_dir()
    {
        std::string result;
        static constexpr DWORD MAX_PATH_LENGTH = 4096;
        result.resize(MAX_PATH_LENGTH);
        auto sz = GetCurrentDirectory(MAX_PATH_LENGTH, result.data());
        result.resize(sz);
        return result;
    }
};
} // namespace common
