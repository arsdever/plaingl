#pragma once

#include <array>
#include <tuple>

namespace
{
template <typename T, size_t C>
struct vertex_attribute
{
    using attribute_data_type = std::array<T, C>;
    using attribute_element_type = T;
    static constexpr size_t attribute_element_count = C;
    static constexpr size_t size = sizeof(T) * C;
    attribute_data_type _data;
};

template <typename... ATTRIBUTES>
struct calculate_combined_size_helper;

template <typename ATTRIBUTE0, typename... ATTRIBUTES>
struct calculate_combined_size_helper<ATTRIBUTE0, ATTRIBUTES...>
{
    static constexpr size_t size =
        calculate_combined_size_helper<ATTRIBUTES...>::size + ATTRIBUTE0::size;
};

template <typename ATTRIBUTE>
struct calculate_combined_size_helper<ATTRIBUTE>
{
    static constexpr size_t size = ATTRIBUTE::size;
};

template <typename... T>
struct reverse_order_tuple_helper;

template <typename... T>
using tuple_cat_t = decltype(std::tuple_cat(std::declval<T>()...));

template <typename T, typename... U>
struct reverse_order_tuple_helper<T, U...>
{
    using type = tuple_cat_t<typename reverse_order_tuple_helper<U...>::type,
                             std::tuple<T>>;
};

template <typename T>
struct reverse_order_tuple_helper<T>
{
    using type = std::tuple<T>;
};

template <typename... ATTRIBUTES>
struct vertex_helper
{
    using tuple_type = typename reverse_order_tuple_helper<ATTRIBUTES...>::type;
    tuple_type _attributes;

    static constexpr size_t attributes_count = std::tuple_size_v<tuple_type>;
    static constexpr size_t size =
        calculate_combined_size_helper<ATTRIBUTES...>::size;
    static constexpr std::array<int, std::tuple_size_v<tuple_type>>
        attribute_element_counts = { ATTRIBUTES::attribute_element_count... };
    static constexpr std::array<int, std::tuple_size_v<tuple_type>>
        attribute_sizes = { ATTRIBUTES::size... };
};
} // namespace

using position_3d_attribute = vertex_attribute<float, 3>;
using normal_3d_attribute = vertex_attribute<float, 3>;
using uv_attribute = vertex_attribute<float, 2>;

struct vertex
{
    using helper_t =
        vertex_helper<position_3d_attribute, normal_3d_attribute, uv_attribute>;
    using tuple_type = helper_t::tuple_type;
    position_3d_attribute& position()
    {
        return std::get<2>(helper._attributes);
    }
    normal_3d_attribute& normal() { return std::get<1>(helper._attributes); }
    uv_attribute& uv() { return std::get<0>(helper._attributes); }

    helper_t helper;
    static constexpr size_t size = helper_t::size;
    static constexpr size_t attributes_count = helper_t::attributes_count;
    static constexpr std::array<int, std::tuple_size_v<helper_t::tuple_type>>
        attribute_elements_counts = helper_t::attribute_element_counts;
    static constexpr std::array<int, std::tuple_size_v<helper_t::tuple_type>>
        attribute_sizes = helper_t::attribute_sizes;
};
