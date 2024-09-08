#include <gtest/gtest.h>

#include "common/directory.hpp"
#include "common/filesystem.hpp"

TEST(Directory, traversing)
{
    auto path = common::filesystem::path::current_dir();
    common::directory dir(std::string(path.full_path()));

    std::vector<std::string> files;
    dir.visit_files([ & ](std::string file, bool is_directory)
    { files.push_back(file); });

    // the value of this variable comes from the buildsystem
    // +2 counts . and ..
    EXPECT_EQ(files.size(),
              DIRECTORY_UNITTEST_FILES_COUNT_IN_CURRENT_DIRECTORY + 2);
}
