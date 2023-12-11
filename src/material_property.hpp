#pragma once

#include <any>
#include <string>

#include <glad/gl.h>

struct material_property
{
    enum class data_type
    {
        unknown,

        type_integer,
        type_integer_vector_2,
        type_integer_vector_3,
        type_integer_vector_4,

        type_unsigned_integer,
        type_unsigned_integer_vector_2,
        type_unsigned_integer_vector_3,
        type_unsigned_integer_vector_4,

        type_float,
        type_float_vector_2,
        type_float_vector_3,
        type_float_vector_4,

        type_image
    };

    /**
     * @brief Property name
     */
    std::string _name;

    /**
     * @brief Property components count
     */
    int _size;

    /**
     * @brief Property data type
     */
    data_type _type;

    /**
     * @brief Property data
     */
    std::any _value;

    /**
     * @brief Special field
     *
     * For textures, this is the OpenGL texture id used in the material.
     */
    unsigned _special;
};
