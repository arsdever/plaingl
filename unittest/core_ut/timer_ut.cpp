#include <gtest/gtest.h>

#include "core/timer.hpp"

TEST(Timer, start_and_execute_once)
{
    core::timer t;
    std::chrono::time_point now = std::chrono::steady_clock::now();

    std::atomic_int value = 0;

    t.tick += [ &value ]() { value.store(value + 1); };
    t.start(std::chrono::milliseconds(100));
    EXPECT_EQ(0, value.load());
    t.wait();
    EXPECT_EQ(1, value.load());
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - now);
    EXPECT_LT(std::abs((duration - std::chrono::milliseconds(100)).count()),
              10);
}

TEST(Timer, start_and_execute_iterations)
{
    core::timer t;
    std::chrono::time_point now = std::chrono::steady_clock::now();

    std::atomic_int value = 0;

    t.tick += [ &value ]() { value.store(value + 1); };
    t.set_iteration(std::chrono::milliseconds(100));
    t.start(std::chrono::milliseconds(50));
    EXPECT_EQ(0, value.load());
    std::this_thread::sleep_for(std::chrono::milliseconds(305));
    t.cancel();
    EXPECT_EQ(3, value.load());
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - now);
    EXPECT_LT(std::abs((duration - std::chrono::milliseconds(305)).count()),
              10);
}

TEST(Timer, single_shot)
{
    std::chrono::time_point now = std::chrono::steady_clock::now();
    std::atomic_int value = 0;
    core::timer::single_shot(std::chrono::milliseconds(100),
                             [ &value ]() { ++value; });
    EXPECT_EQ(1, value.load());
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - now);
    EXPECT_LT(std::abs((duration - std::chrono::milliseconds(100)).count()),
              10);
}
