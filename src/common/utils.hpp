#pragma once

struct string_hash
{
    using is_transparent = void; // enables heterogenous lookup

    std::size_t operator()(const char* char_string) const
    {
        std::hash<std::string_view> hasher;
        return hasher(char_string);
    }

    std::size_t operator()(const std::string& string) const
    {
        std::hash<std::string_view> hasher;
        return hasher(string);
    }

    std::size_t operator()(std::string_view string_view) const
    {
        std::hash<std::string_view> hasher;
        return hasher(string_view);
    }
};

// geometry utils

template <typename T>
bool is_in_range(T rb, T re, T v)
{
    return v >= rb && v <= re;
}

template <typename T>
bool rect_contains(glm::vec<2, T, glm::defaultp> top_left,
                   glm::vec<2, T, glm::defaultp> bottom_right,
                   glm::vec<2, T, glm::defaultp> pos)
{
    glm::vec<2, T, glm::defaultp> tl = top_left;
    glm::vec<2, T, glm::defaultp> br = bottom_right;
    if (top_left.x > bottom_right.x)
    {
        tl.x = bottom_right.x;
        br.x = top_left.x;
    }
    if (top_left.y > bottom_right.y)
    {
        tl.y = bottom_right.y;
        br.y = top_left.y;
    }

    return is_in_range(tl.x, br.x, pos.x) && is_in_range(tl.y, br.y, pos.y);
}

template <typename R, typename... ARGS>
R gl_convert(ARGS... args);

template <typename... T>
struct deduce_function_type;

template <typename R, typename C, typename... ARGS>
struct deduce_function_type<R (C::*)(ARGS...) const>
{
    using type = R (*)(ARGS...);
    using return_type = R;
    static constexpr bool is_function = true;
};

template <typename R, typename... ARGS>
struct deduce_function_type<std::function<R(ARGS...)>>
{
    using type = R (*)(ARGS...);
    using return_type = R;
    static constexpr bool is_function = true;
};

template <typename T>
struct function_info
{
    using resolver = std::conditional_t<requires {
        &T::operator();
    }, deduce_function_type<decltype(&T::operator())>, deduce_function_type<T>>;
    using type = resolver::type;
    using return_type = resolver::return_type;
};

template <typename R, typename... ARGS>
struct deduce_function_type<R(ARGS...)>
{
    using type = R (*)(ARGS...);
    using return_type = R;
    static constexpr bool is_function = true;
};

template <typename T, int I, typename... ARGS>
struct variant_index_helper;

template <typename T, int I, typename... ARGS>
struct variant_index_helper<T, I, std::variant<ARGS...>>
{
    static constexpr int value =
        std::is_same_v<T, std::variant_alternative_t<I, std::variant<ARGS...>>>
            ? I
            : variant_index_helper<T, I - 1, std::variant<ARGS...>>::value;
};

template <typename T, typename... ARGS>
struct variant_index_helper<T, -1, std::variant<ARGS...>>
{
    static constexpr int value = -1;
};

template <typename V, typename T>
struct variant_index
{
    static constexpr int value =
        variant_index_helper<T, std::variant_size_v<V> - 1, V>::value;
};

template <typename V, typename T>
constexpr int variant_index_v = variant_index<V, T>::value;

template <typename T>
struct is_glm_type : std::false_type
{
};

template <size_t N, typename T, glm::qualifier Q>
struct is_glm_type<glm::vec<N, T, Q>> : std::true_type
{
};

template <typename T, glm::qualifier Q>
struct is_glm_type<glm::qua<T, Q>> : std::true_type
{
};

template <size_t N, size_t M, typename T, glm::qualifier Q>
struct is_glm_type<glm::mat<N, M, T, Q>> : std::true_type
{
};

template <typename T>
static constexpr bool is_glm_type_v = is_glm_type<T>::value;

template <typename T, typename U>
struct glm_typecast;

template <size_t N, typename T, typename U, glm::qualifier Q>
struct glm_typecast<glm::vec<N, T, Q>, U>
{
    using type = glm::vec<N, U, Q>;
};

template <typename T, typename U, glm::qualifier Q>
struct glm_typecast<glm::qua<T, Q>, U>
{
    using type = glm::qua<U, Q>;
};

template <size_t N, size_t M, typename T, typename U, glm::qualifier Q>
struct glm_typecast<glm::mat<N, M, T, Q>, U>
{
    using type = glm::mat<N, M, U, Q>;
};

template <typename T, typename U>
using glm_typecast_t = typename glm_typecast<T, U>::type;

template <class Base>
class enable_shared_from_base : public std::enable_shared_from_this<Base>
{
public:
    template <class T = Base>
    std::shared_ptr<T> shared_from_this()
    {
        if constexpr (std::is_same_v<T, Base>)
        {
            return std::enable_shared_from_this<Base>::shared_from_this();
        }
        return std::static_pointer_cast<T>(shared_from_this<Base>());
    }
};
