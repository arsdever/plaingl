#include <gtest/gtest.h>

#include "assets/assets_fwd.hpp"

#include "assets/asset_cache.hpp"
#include "assets/asset_manager.hpp"
#include "assets/type_importer.hpp"
#include "common/file.hpp"
#include "common/filesystem.hpp"
#include "common/logging.hpp"
#include "common/main_thread_dispatcher.hpp"

using assets::asset_manager;
using assets::type_importer;

struct test_asset_type
{
    std::string body;
};

class test_asset_importer : public type_importer<test_asset_type>
{
protected:
    void initialize_asset(assets::asset& asset) override
    {
        asset.get_raw_data() = std::make_shared<test_asset_type>();
    }

    void read_asset_data(std::string_view asset_file) override
    {
        get_logger("test_loader")->info("Updating {}", asset_file);
        _data->body = std::move(common::file::read_all(asset_file));
    }
};

class TestAssetManager : public ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        common::main_thread_dispatcher::initialize();
        asset_manager::initialize(
            (common::filesystem::path::current_dir() / "data").full_path());
        asset_manager::register_importer(
            ".test", std::make_shared<test_asset_importer>());
        asset_manager::scan_project_directory();
        asset_manager::setup_project_directory_watch();
    }

    static void TearDownTestSuite()
    {
        asset_manager::shutdown();
        common::main_thread_dispatcher::shutdown();
    }
};

TEST_F(TestAssetManager, project_folder_scan)
{
    using namespace assets;

    auto ta =
        asset_manager::get<test_asset_type>("test_asset_manager_load.test");
    EXPECT_EQ(ta->body, "Hello world !!!\n");
}

TEST_F(TestAssetManager, resource_reloading)
{
    using namespace assets;

    auto ta = asset_manager::get<test_asset_type>("test_asset_manager.test");
    EXPECT_EQ(ta->body, "Hello world !!!\n");

    common::file::remove("data/test_asset_manager.test");
    common::file::write("data/test_asset_manager.test", "Hello world 2 !!!\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    common::main_thread_dispatcher::run_all();
    EXPECT_EQ(ta->body, "Hello world 2 !!!\n");

    common::file::remove("data/test_asset_manager.test");
    common::file::write("data/test_asset_manager.test", "Hello world !!!\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    common::main_thread_dispatcher::run_all();
    EXPECT_EQ(ta->body, "Hello world !!!\n");
}
