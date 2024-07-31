#include <gtest/gtest.h>

#include "common/file.hpp"

using file = common::file;

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

TEST(File, read_all_ignore_newline_modifications)
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

// TODO: add testcase for changed
