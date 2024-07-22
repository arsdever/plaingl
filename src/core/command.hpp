#pragma once

namespace core
{
class command_base
{
public:
    virtual ~command_base() = default;
    virtual void execute() = 0;
};

template <typename... ARGS>
class command : public command_base
{
public:
    command(std::tuple<ARGS&&...> args)
        : _args(std::move(args))
    {
    }

    template <size_t N>
    std::tuple_element_t<N, std::tuple<ARGS...>>& get()
    {
        return std::get<N>(_args);
    }

    template <size_t N>
    const std::tuple_element_t<N, std::tuple<ARGS...>>& get() const
    {
        return std::get<N>(_args);
    }

private:
    std::tuple<ARGS...> _args;
};

template <>
class command<> : public command_base
{
public:
    virtual void execute() = 0;
};
} // namespace core
