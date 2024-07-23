#pragma once

namespace core
{
class command_dispatcher;
class command_base;
} // namespace core

class console
{
public:
    console();
    ~console();

    void input(int in);
    void input(std::string in);

    bool is_active() const;
    void activate();
    void deactivate();

    core::command_dispatcher& processor();

    static std::vector<std::string_view>
    tokenize(std::string_view in, int (*delim)(int) = &std::isspace);

    void register_command(std::string_view command_path,
                          std::function<std::shared_ptr<core::command_base>(
                              const std::vector<std::string_view>&)> func);

    template <typename CMD, typename... ARGS>
    void register_command(std::string_view command_path);

    void register_for_logs();

    std::string_view get_current_input() const;

    template <size_t N>
    std::array<std::string_view, N> history() const
    {
        std::array<std::string_view, N> result;
        for (int i = 0; i < N; ++i)
        {
            result[ i ] = history_element(i);
        }

        return result;
    }

private:
    void add_history(std::string_view text);
    std::string_view history_element(int index) const;

    template <typename T>
    static T convert(std::string_view in);

    template <size_t N, typename... ARGS>
    static void
    populate_arguments_impl(std::tuple<ARGS...>& args,
                            const std::vector<std::string_view>& param_tokens)
    {
        std::get<N>(args) =
            convert<std::tuple_element_t<N, std::tuple<ARGS...>>>(
                param_tokens[ N ]);

        if constexpr (N < sizeof...(ARGS) - 1)
            populate_arguments_impl<N + 1, ARGS...>(args, param_tokens);
    }

    template <typename... ARGS>
    static void
    populate_arguments(std::tuple<ARGS...>& args,
                       const std::vector<std::string_view>& param_tokens)
    {
        populate_arguments_impl<0, ARGS...>(args, param_tokens);
    }

    struct impl;
    std::unique_ptr<impl> _impl;
};

template <typename CMD, typename... ARGS>
void console::register_command(std::string_view command_path)
{
    register_command(command_path,
                     [](const std::vector<std::string_view>& param_tokens)
                         -> std::shared_ptr<core::command_base>
    {
        if constexpr (sizeof...(ARGS) != 0)
        {
            std::tuple<ARGS...> args = {};
            console::populate_arguments<ARGS...>(args, param_tokens);

            return std::make_shared<CMD>(std::move(args));
        }
        else
        {
            return std::make_shared<CMD>();
        }
    });
}
