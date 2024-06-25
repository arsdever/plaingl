#pragma once

#include <utility>

#include "graphics/commands/commands.hpp"

namespace graphics
{
namespace internal
{
namespace utils
{
#pragma region Collects elements of enum class into a single array
template <size_t N, typename E>
struct enum_to_array_helper
{
    static constexpr std::array<E, N> value = {
        enum_to_array_helper<N - 1, E>::value, static_cast<E>(N)
    };
};
template <typename E>
struct enum_to_array_helper<0, E>
{
    static constexpr E value = static_cast<E>(0);
};

template <typename E>
struct enum_to_array_helper<static_cast<size_t>(E::count), E>
{
    static constexpr size_t count = static_cast<size_t>(E::count);
    static constexpr std::array<E, count> value = {
        enum_to_array_helper<count - 1, E>::value
    };
};

template <typename E>
struct enum_to_array
{
    using unpacker = enum_to_array_helper<static_cast<size_t>(E::count), E>;
    static constexpr std::array<E, unpacker::count> value = unpacker::value;
};

#pragma endregion

template <typename... T>
struct tuple_from_array_via_typetraits;

template <typename A, std::size_t... I>
struct tuple_from_array_via_typetraits<A, std::index_sequence<I...>>
{
    using type = std::tuple<
        typename command_type_traits<std::get<I>(A::value)>::type...>;
};

template <typename A>
struct commands_type_traits_from_array
{
    using type = tuple_from_array_via_typetraits<
        A,
        std::make_index_sequence<A::value.size()>>::type;
};

template <typename... ARGS>
struct tuple_to_variant
{
    using type = std::variant<ARGS...>;
};

template <typename T>
struct command_args_from_enum
{
    using type = typename tuple_to_variant<commands_type_traits_from_array<
        enum_to_array<command_type>>::type>::type;
};
} // namespace utils

template <command_type... types>
using command_arg = std::variant<typename command_type_traits<types>::type...>;

using possible_command_arg = utils::command_args_from_enum<command_type>::type;
} // namespace internal

struct command
{
    command_type _type {};
    internal::possible_command_arg _arguments {};
};
} // namespace graphics
