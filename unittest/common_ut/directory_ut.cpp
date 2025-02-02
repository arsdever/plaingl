#include <gtest/gtest.h>

#include "common/directory.hpp"
#include "common/filesystem.hpp"

TEST(Directory, traversing)
{
    auto path = common::filesystem::path::current_dir();
    common::directory dir(std::string(path.full_path()));

    static constexpr std::array<
        std::string_view,
        DIRECTORY_UNITTEST_FILES_COUNT_IN_CURRENT_DIRECTORY>
        expected = { DIRECTORY_UNITTEST_FILES_IN_CURRENT_DIRECTORY };

    std::vector<std::string> files;
    dir.visit_files([ & ](std::string file, bool is_directory)
    { files.push_back(std::string(common::filesystem::path(file).full_path())); });

    // the value of this variable comes from the buildsystem
    // +2 counts . and ..
    EXPECT_EQ(files.size(),
              DIRECTORY_UNITTEST_FILES_COUNT_IN_CURRENT_DIRECTORY + 2);

    for (size_t i = 0; i < expected.size(); i++)
    {
        EXPECT_TRUE(std::find(files.begin(), files.end(), expected[ i ]) !=
                    files.end())
            << "Could not find " << expected[ i ];
    }
}
