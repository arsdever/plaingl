#pragma once

namespace graphics
{
struct command;

namespace utils
{
template <typename E>
constexpr size_t enum_count = static_cast<size_t>(E::count);

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

template <typename T, size_t... I>
inline static constexpr std::array<T, sizeof...(I)>
construct_array_for_enum(std::index_sequence<I...> is)
{
    return { static_cast<T>(I)... };
}

template <typename E>
struct enum_to_array
{
    using unpacker = enum_to_array_helper<static_cast<size_t>(E::count), E>;
    static constexpr std::array<E, unpacker::count> value =
        construct_array_for_enum<E>(
            std::make_index_sequence<unpacker::count>());
};

template <typename T,
          size_t N = enum_count<T>,
          std::array<T, enum_count<T>> types = enum_to_array<T>::value,
          size_t... I>
struct executor_getter_helper
{
    static constexpr std::array<void (*)(command&), N> value = {
        executor_getter_helper<T, N - 1, types>::value[ I ]...,
        std::array<void (*)(command&), 1> {
            internal::command_type_traits<std::get<N - 1>(types)>::executor }
    };
};

template <typename T, std::array<T, enum_count<T>> types>
struct executor_getter_helper<T, 0, types>
{
    static constexpr std::array<void (*)(command&), 0> value = {
        internal::command_type_traits<std::get<0>(types)>::executor
    };
};

template <typename T,
          std::array<T, utils::enum_count<T>> types = enum_to_array<T>::value>
struct executors
{
    static constexpr std::array<void (*)(command&), utils::enum_count<T>>
        value = executor_getter_helper<T, utils::enum_count<T>, types>::value;
};
} // namespace utils
} // namespace graphics
