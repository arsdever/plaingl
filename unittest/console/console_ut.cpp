#include <gtest/gtest.h>

#include "console/console.hpp"
#include "core/command.hpp"
#include "core/command_dispatcher.hpp"

TEST(Console, tokenize)
{
    EXPECT_EQ(console::tokenize("Hello World"),
              (std::vector<std::string_view> { "Hello", "World" }));
    EXPECT_EQ(console::tokenize("  \t   \n   \r\n   \r   Hello World"),
              (std::vector<std::string_view> { "Hello", "World" }));
    EXPECT_EQ(console::tokenize("Hello World  \t   \n   \r\n   \r   "),
              (std::vector<std::string_view> { "Hello", "World" }));
    EXPECT_EQ(console::tokenize("    \t   \n   \r\n   \r   "),
              (std::vector<std::string_view> {}));
}

TEST(Console, empty_string)
{
    console c;
    c.input("\n\n\n\n");
    c.processor().execute_all();

    c.input("   \n");
    c.processor().execute_all();

    c.input('\n');
    c.processor().execute_all();

    c.input(GLFW_KEY_ENTER);
    c.processor().execute_all();

    c.input("   ");
    c.input(GLFW_KEY_ENTER);
    c.processor().execute_all();
}

TEST(Console, register_command)
{
    static int my_variable = 0;
    class my_own_command : public core::command<int>
    {
    public:
        using command::command;
        void execute() override { my_variable = get<0>(); }
    };

    class my_own_command1 : public core::command<int>
    {
    public:
        using command::command;
        void execute() override { my_variable -= get<0>(); }
    };

    class my_own : public core::command<int>
    {
    public:
        using command::command;
        void execute() override { my_variable /= get<0>(); }
    };

    class exit : public core::command<>
    {
    public:
        void execute() override
        {
            std::cout << "Finishing the test" << std::endl;
        }
    };

    console c;
    c.register_command<my_own_command, int>("my.own.command");
    c.register_command<my_own_command1, int>("my.own.command1");
    c.register_command<my_own, int>("my.own");
    c.register_command<exit>("exit");
    c.input("my own command 123\nmy own command1 23\nmy own 25\nexit\n");

    c.processor().execute_all();

    EXPECT_EQ(my_variable, 4);
}
