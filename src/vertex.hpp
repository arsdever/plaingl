#pragma once

namespace
{

template <typename T, size_t C, typename S = std::array<T, C>>
struct vertex_attribute
{
    using attribute_data_storage_type = S;
    using attribute_component_type = T;

    static constexpr size_t component_count = C;
    static constexpr size_t size = sizeof(T) * C;

    attribute_data_storage_type data;
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

template <typename... T>
using reverse_order_tuple = reverse_order_tuple_helper<T...>::type;

} // namespace

using position_3d_attribute = vertex_attribute<float, 3, glm::vec3>;
using position_2d_attribute = vertex_attribute<float, 2, glm::vec2>;
using normal_3d_attribute = vertex_attribute<float, 3, glm::vec3>;
using color_attribute = vertex_attribute<float, 4, glm::vec4>;
using uv_attribute = vertex_attribute<float, 2, glm::vec2>;

template <typename... ATTRIBUTES>
struct vertex
{
    using tuple_type = reverse_order_tuple<ATTRIBUTES...>;

    static constexpr size_t attribute_count = std::tuple_size_v<tuple_type>;
    static constexpr size_t size =
        calculate_combined_size_helper<ATTRIBUTES...>::size;
    static constexpr std::array<int, std::tuple_size_v<tuple_type>>
        attribute_component_counts = { ATTRIBUTES::component_count... };
    static constexpr std::array<int, std::tuple_size_v<tuple_type>>
        attribute_sizes = { ATTRIBUTES::size... };

    tuple_type _attributes;

    template <size_t I>
    std::tuple_element<I, tuple_type>::type::attribute_data_storage_type& get()
    {
        return std::get<I>(_attributes).data;
    }

    static void activate_attributes()
    {
        for (int i = 0; i < attribute_count; ++i)
        {
            glEnableVertexAttribArray(i);
        }
    }

    static void initialize_attributes()
    {
        size_t attribute_offset = 0;
        for (int i = 0; i < attribute_count; ++i)
        {
            glVertexAttribPointer(i,
                                  attribute_component_counts[ i ],
                                  GL_FLOAT,
                                  GL_FALSE,
                                  size,
                                  (void*)attribute_offset);
            attribute_offset += attribute_sizes[ i ];
        }
    }
};

struct vertex3d
    : vertex<position_3d_attribute, normal_3d_attribute, uv_attribute>
{
    position_3d_attribute::attribute_data_storage_type& position()
    {
        return get<2>();
    }
    normal_3d_attribute::attribute_data_storage_type& normal()
    {
        return get<1>();
    }
    uv_attribute::attribute_data_storage_type& uv() { return get<0>(); }
};

struct simple_vertex2d : vertex<position_2d_attribute>
{
    position_2d_attribute::attribute_data_storage_type& position()
    {
        return get<0>();
    }
};

struct simple_vertex3d : vertex<position_3d_attribute>
{
    position_3d_attribute::attribute_data_storage_type& position()
    {
        return get<0>();
    }
};

struct colored_vertex2d : vertex<position_2d_attribute, color_attribute>
{
    position_2d_attribute::attribute_data_storage_type& position()
    {
        return get<1>();
    }

    color_attribute::attribute_data_storage_type& color() { return get<0>(); }
};
