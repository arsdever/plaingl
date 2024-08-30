#include <gtest/gtest.h>

#include "common/file.hpp"
#include "common/filesystem.hpp"

using file = common::file;
namespace fs = common::filesystem;

TEST(File, read_all_directly)
{
    {
        file f { "test.txt" };
        EXPECT_EQ("Hello world !!!\n", f.read_all());
    }

    {
        EXPECT_EQ("Hello world !!!\n", file::read_all("test.txt"));
    }
}

TEST(File, read_as_binary_file)
{
    file f { "test.txt" };
    std::string contents = "Hello world !!!\r\n";
    EXPECT_EQ(std::vector<char>(contents.begin(), contents.end()),
              f.read_all<std::vector<char>>());
}

TEST(File, seek_read)
{
    file f { "test.txt" };
    f.seek(5);
    EXPECT_EQ("Hello world !!!\n", f.read_all());
    EXPECT_NE(" world", f.read_all());
    f.seek(5);
    EXPECT_EQ(" world", f.read(6));
}

TEST(File, read_structure)
{
    file f { "struct_content.hex" };
    struct content
    {
        bool b;
        char c;
        short s;
        int i;
    };

    auto v = f.template read<std::vector<content>>(5);

    EXPECT_EQ(1, v.size());
    EXPECT_EQ(true, v[ 0 ].b);
    EXPECT_EQ('H', v[ 0 ].c);
    EXPECT_EQ(53, v[ 0 ].s);
    EXPECT_EQ(232345, v[ 0 ].i);
}

TEST(File, read_incomplete_structure)
{
    file f { "incomplete_struct_content.hex" };
    struct content
    {
        bool b;
        char c;
        short s;
        int i;
    };

    auto v = f.template read<std::vector<content>>(5);

    EXPECT_EQ(2, v.size());
    EXPECT_EQ(true, v[ 0 ].b);
    EXPECT_EQ('H', v[ 0 ].c);
    EXPECT_EQ(53, v[ 0 ].s);
    EXPECT_EQ(232345, v[ 0 ].i);
    EXPECT_EQ(0, v[ 1 ].b);
    EXPECT_EQ('W', v[ 1 ].c);
    EXPECT_EQ(0x1510, v[ 1 ].s);
    EXPECT_EQ(0x30201510, v[ 1 ].i);
}

TEST(File, open_and_close)
{
    file f { "test.txt" };
    EXPECT_FALSE(f.is_open());
    f.open(file::open_mode::read);
    EXPECT_TRUE(f.is_open());
    f.close();
    EXPECT_FALSE(f.is_open());
}

TEST(File, prevent_multiple_open)
{
    file f { "test.txt" };
    f.open(file::open_mode::read);
    file f2 { "test.txt" };
    f2.open(file::open_mode::read);

    EXPECT_TRUE(f.is_open());
    EXPECT_FALSE(f2.is_open());
}

TEST(File, close_on_destruction)
{
    {
        file f { "test.txt" };
        f.open(file::open_mode::read);
        EXPECT_TRUE(f.is_open());
    }

    file f { "test.txt" };
    f.open(file::open_mode::read);
    EXPECT_TRUE(f.is_open());
}

TEST(File, move)
{
    file f { "test.txt" };
    f.open(file::open_mode::read);
    EXPECT_EQ("Hello", f.read(5));
    file f2 { std::move(f) };
    EXPECT_FALSE(f.is_open());
    EXPECT_TRUE(f2.is_open());
    EXPECT_EQ(" world", f2.read(6));
}

TEST(File, check_filepath)
{
    file f { "test.txt" };
    EXPECT_EQ("test.txt", f.get_filepath());
    file f1 { "non_existing_file.extension" };
    EXPECT_EQ("non_existing_file.extension", f1.get_filepath());
}

TEST(File, create_and_remove)
{
    file f { "create_and_remove.txt" };
    EXPECT_FALSE(f.exists());
    f.open(file::open_mode::read);
    EXPECT_FALSE(f.exists());
    EXPECT_FALSE(f.is_open());
    f.open(file::open_mode::write);
    EXPECT_TRUE(f.exists());
    EXPECT_TRUE(f.is_open());
    f.remove();
    EXPECT_FALSE(f.is_open());
    EXPECT_FALSE(f.exists());
}

TEST(File, change_content)
{
    file f { "change_content.txt" };
    f.open(file::open_mode::read_write);
    f.write("Hello world !!!\n");
    f.seek(0);
    EXPECT_EQ("Hello world !!!\n", f.read_all());
    f.seek(0);
    f.write("Goodbye");
    f.seek(0);
    EXPECT_EQ("Goodbyeorld !!!\n", f.read_all());
    f.remove();
    EXPECT_FALSE(f.exists());
}

TEST(File, write_structures)
{
    struct content
    {
        bool b;
        char c;
        short s;
        int i;

        bool operator==(const content& o) const
        {
            return b == o.b && c == o.c && s == o.s && i == o.i;
        }
    };

    file f { "write_structures.hex" };
    f.open(file::open_mode::read_write);

    std::vector<content> v;
    v.push_back({ true, 'H', 53, 232345 });
    v.push_back({ false, 'W', 0x1510, 0x30201510 });

    f.write(v);
    f.seek(0);

    std::vector<content> v2 = f.template read_all<std::vector<content>>();
    EXPECT_EQ(v, v2);

    f.remove();
}

TEST(File, direct_file_access)
{
    static constexpr auto filename = "direct_file_access.txt";
    file::write(filename, "Hello world !!!\n");
    EXPECT_TRUE(file::exists(filename));
    EXPECT_EQ("Hello world !!!\n", file::read_all(filename));
    file::append(filename, "Goodbye world\n");
    EXPECT_EQ("Hello world !!!\nGoodbye world\n", file::read_all(filename));
    file::remove(filename);
    EXPECT_FALSE(file::exists(filename));
}

TEST(File, watch_for_changes)
{
    size_t counter = 0;
    size_t check_counter = 0;
    file f { "watch_for_changes.txt" };
    f.changed += [ &counter ](auto type) { ++counter; };
    f.open(file::open_mode::write);
    f.write("Hello world !!!\n");
    EXPECT_EQ(counter, ++check_counter);
    f.seek(0);
    f.write("Goodbye");
    f.close();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(counter, ++check_counter);
    f.remove();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(counter, ++check_counter);
}

TEST(File, file_path)
{
    fs::path p { "test.txt" };
    EXPECT_EQ("test.txt", p.filename());
    EXPECT_EQ("test", p.stem());
    EXPECT_EQ(".txt", p.extension());
    EXPECT_EQ("common_ut", p.directory());

    fs::path p1 { "non_existing_file.extension" };
    EXPECT_EQ("non_existing_file.extension", p1.filename());
    EXPECT_EQ("non_existing_file", p1.stem());
    EXPECT_EQ(".extension", p1.extension());
    EXPECT_EQ("common_ut", p1.directory());

    fs::path p2 { "./hello\\..\\\\dir/..\\/another_dir\\../directory/"
                  "non_existing_file.extension" };
    EXPECT_EQ("non_existing_file.extension", p2.filename());
    EXPECT_EQ("non_existing_file", p2.stem());
    EXPECT_EQ(".extension", p2.extension());
    EXPECT_EQ("directory", p2.directory());

    auto cd = fs::path::current_dir();
    EXPECT_EQ(cd.full_path(), p.full_path_without_filename());
    EXPECT_EQ(cd.full_path(), p1.full_path_without_filename());
}

TEST(File, file_path_operations)
{
    fs::path p { "test.txt" };
    EXPECT_EQ("test.txt", p.filename());
    EXPECT_NE("test.txt", p.directory());
    EXPECT_EQ("test", (p + "text").stem());
    EXPECT_EQ("test.txt", (p / "text").directory());
    EXPECT_EQ("test.txt", (p / "text/..").filename());
}
