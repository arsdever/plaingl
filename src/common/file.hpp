#pragma once

#include "common/event.hpp"
#include "common/file_watcher.hpp"

namespace common
{
class file
{
public:
    /**
     * @brief File open mode
     */
    enum class open_mode
    {
        not_open = 0,
        read = 1,
        write = 2,
        read_write = read | write,
        append = 4,
    };

public:
    file(std::string path);
    file(const file&) = delete;
    file& operator=(const file&) = delete;
    file(file&& o);
    file& operator=(file&& o);
    ~file();

    /**
     * @brief Open the file
     *
     * @param mode the open mode
     */
    void open(open_mode mode);

    /**
     * @brief Close the file
     */
    void close();

    /**
     * @brief Remove the file
     */
    void remove();

    /**
     * @brief Set the cursor position
     * @param pos the new cursor position from the beginning of the file
     */
    void seek(size_t pos);

    /**
     * @brief Get the file size in bytes
     * @return size_t the file size
     */
    size_t get_size() const;

    /**
     * @brief Check if the file is open
     * @return true if the file is open
     */
    bool is_open() const;

    /**
     * @brief Check if the file is a directory
     * @return true if the file is a directory
     */
    bool is_directory() const;

    /**
     * @brief Check if the file exists
     * @return true if the file exists
     */
    bool exists() const;

    /**
     * @brief Get the file path
     * @return std::string_view the file path
     */
    std::string_view get_filepath() const;

    /**
     * @brief Read some bytes of the file.
     *
     * @note Reading starts at the current cursor position.
     *
     * @param buffer a buffer into which the data will be read
     * @param length the number of bytes to be read
     * @return size_t the number of bytes actually read
     */
    size_t read(char* buffer, size_t length);

    /**
     * @brief Read some content of the file.
     *
     * @note Reading starts at the current cursor position.
     *
     * @tparam T a container type into which the data will be read
     * @param length the number of elements to be read
     * @return T the elements actually read
     */
    template <typename T = std::string>
    T read(size_t length);

    /**
     * @brief Read all the content of the file.
     *
     * @note After reading the cursor will be placed at the end of the file.
     *
     * @return std::string the content of the file
     */
    template <typename T = std::string>
    T read_all();

    size_t write(const char* buffer, size_t length);

    template <typename T = std::string>
    size_t write(const T& data);

    inline size_t write(const char* data)
    {
        return write(std::string_view(data));
    }

    // TODO: implement
    event<void(file_change_type)> changed;

    static file create(std::string_view path, std::string_view contents = "");
    static void remove(std::string_view path);
    template <typename T = std::string>
    static T read_all(std::string_view path);
    template <typename T = std::string>
    static size_t write(std::string_view path, const T& data);
    inline static size_t write(std::string_view path, const char* data)
    {
        return write(path, std::string_view(data));
    }
    template <typename T = std::string>
    static size_t append(std::string_view path, const T& data);
    static bool exists(std::string_view path);

private:
    size_t read_data(char* buffer, size_t length);

private:
    struct impl;
    std::unique_ptr<impl> _impl;
    file_watcher _watcher;
};

template <typename T>
T file::read(size_t length)
{
    T result;
    result.resize(length);

    static constexpr size_t result_element_size =
        sizeof(typename T::value_type);

    size_t sz = read(reinterpret_cast<char*>(result.data()),
                     length * result_element_size);

    result.resize(sz / result_element_size);

    if constexpr (std::is_same_v<std::string, T>)
    {
        std::string::size_type pos = 0; // Must initialize
        while ((pos = result.find("\r\n", pos)) != std::string::npos)
        {
            result.erase(pos, 1);
        }
    }

    return result;
}

template <typename T>
T file::read_all()
{
    if (!is_open())
        open(open_mode::read);

    static constexpr size_t result_element_size =
        sizeof(typename T::value_type);
    auto length = get_size() / result_element_size;

    return read<T>(length);
}

template <typename T>
size_t file::write(const T& data)
{
    static constexpr size_t result_element_size =
        sizeof(typename T::value_type);
    auto length = data.size() * result_element_size;

    return write(reinterpret_cast<const char*>(data.data()), length);
}

template <typename T>
T file::read_all(std::string_view path)
{
    file f { std::string(path) };
    return f.read_all<T>();
}

template <typename T>
size_t file::write(std::string_view path, const T& data)
{
    file f { std::string(path) };
    f.open(open_mode::write);
    return f.write(data);
}

template <typename T>
size_t file::append(std::string_view path, const T& data)
{
    file f { std::string(path) };
    f.open(open_mode::append);
    return f.write(data);
}
} // namespace common
