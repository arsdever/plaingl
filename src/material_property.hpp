#pragma once

#include <any>
#include <string>

#include <glad/gl.h>

struct material_property_info
{
    enum type
    {
        // supported types
        property_type_1f = GL_FLOAT,
        property_type_2f = GL_FLOAT_VEC2,
        property_type_3f = GL_FLOAT_VEC3,
        property_type_4f = GL_FLOAT_VEC4,

        property_type_1i = GL_INT,
        property_type_2i = GL_INT_VEC2,
        property_type_3i = GL_INT_VEC3,
        property_type_4i = GL_INT_VEC4,

        property_type_1ui = GL_UNSIGNED_INT,
        property_type_2ui = GL_UNSIGNED_INT_VEC2,
        property_type_3ui = GL_UNSIGNED_INT_VEC3,
        property_type_4ui = GL_UNSIGNED_INT_VEC4,

        // unsupported types
        property_type_double = GL_DOUBLE,
        property_type_dvec2 = GL_DOUBLE_VEC2,
        property_type_dvec3 = GL_DOUBLE_VEC3,
        property_type_dvec4 = GL_DOUBLE_VEC4,
        property_type_bool = GL_BOOL,
        property_type_bvec2 = GL_BOOL_VEC2,
        property_type_bvec3 = GL_BOOL_VEC3,
        property_type_bvec4 = GL_BOOL_VEC4,
        property_type_mat2 = GL_FLOAT_MAT2,
        property_type_mat3 = GL_FLOAT_MAT3,
        property_type_mat4 = GL_FLOAT_MAT4,
        property_type_mat2x3 = GL_FLOAT_MAT2x3,
        property_type_mat2x4 = GL_FLOAT_MAT2x4,
        property_type_mat3x2 = GL_FLOAT_MAT3x2,
        property_type_mat3x4 = GL_FLOAT_MAT3x4,
        property_type_mat4x2 = GL_FLOAT_MAT4x2,
        property_type_mat4x3 = GL_FLOAT_MAT4x3,
        property_type_dmat2 = GL_DOUBLE_MAT2,
        property_type_dmat3 = GL_DOUBLE_MAT3,
        property_type_dmat4 = GL_DOUBLE_MAT4,
        property_type_dmat2x3 = GL_DOUBLE_MAT2x3,
        property_type_dmat2x4 = GL_DOUBLE_MAT2x4,
        property_type_dmat3x2 = GL_DOUBLE_MAT3x2,
        property_type_dmat3x4 = GL_DOUBLE_MAT3x4,
        property_type_dmat4x2 = GL_DOUBLE_MAT4x2,
        property_type_dmat4x3 = GL_DOUBLE_MAT4x3,
        property_type_sampler1D = GL_SAMPLER_1D,
        property_type_sampler2D = GL_SAMPLER_2D,
        property_type_sampler3D = GL_SAMPLER_3D,
        property_type_samplerCube = GL_SAMPLER_CUBE,
        property_type_sampler1DShadow = GL_SAMPLER_1D_SHADOW,
        property_type_sampler2DShadow = GL_SAMPLER_2D_SHADOW,
        property_type_sampler1DArray = GL_SAMPLER_1D_ARRAY,
        property_type_sampler2DArray = GL_SAMPLER_2D_ARRAY,
        property_type_sampler1DArrayShadow = GL_SAMPLER_1D_ARRAY_SHADOW,
        property_type_sampler2DArrayShadow = GL_SAMPLER_2D_ARRAY_SHADOW,
        property_type_sampler2DMS = GL_SAMPLER_2D_MULTISAMPLE,
        property_type_sampler2DMSArray = GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
        property_type_samplerCubeShadow = GL_SAMPLER_CUBE_SHADOW,
        property_type_samplerBuffer = GL_SAMPLER_BUFFER,
        property_type_sampler2DRect = GL_SAMPLER_2D_RECT,
        property_type_sampler2DRectShadow = GL_SAMPLER_2D_RECT_SHADOW,
        property_type_isampler1D = GL_INT_SAMPLER_1D,
        property_type_isampler2D = GL_INT_SAMPLER_2D,
        property_type_isampler3D = GL_INT_SAMPLER_3D,
        property_type_isamplerCube = GL_INT_SAMPLER_CUBE,
        property_type_isampler1DArray = GL_INT_SAMPLER_1D_ARRAY,
        property_type_isampler2DArray = GL_INT_SAMPLER_2D_ARRAY,
        property_type_isampler2DMS = GL_INT_SAMPLER_2D_MULTISAMPLE,
        property_type_isampler2DMSArray = GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        property_type_isamplerBuffer = GL_INT_SAMPLER_BUFFER,
        property_type_isampler2DRect = GL_INT_SAMPLER_2D_RECT,
        property_type_usampler1D = GL_UNSIGNED_INT_SAMPLER_1D,
        property_type_usampler2D = GL_UNSIGNED_INT_SAMPLER_2D,
        property_type_usampler3D = GL_UNSIGNED_INT_SAMPLER_3D,
        property_type_usamplerCube = GL_UNSIGNED_INT_SAMPLER_CUBE,
        property_type_usampler1DArray = GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
        property_type_usampler2DArray = GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
        property_type_usampler2DMS = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
        property_type_usampler2DMSArray =
            GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        property_type_usamplerBuffer = GL_UNSIGNED_INT_SAMPLER_BUFFER,
        property_type_usampler2DRect = GL_UNSIGNED_INT_SAMPLER_2D_RECT,
        property_type_image1D = GL_IMAGE_1D,
        property_type_image2D = GL_IMAGE_2D,
        property_type_image3D = GL_IMAGE_3D,
        property_type_image2DRect = GL_IMAGE_2D_RECT,
        property_type_imageCube = GL_IMAGE_CUBE,
        property_type_imageBuffer = GL_IMAGE_BUFFER,
        property_type_image1DArray = GL_IMAGE_1D_ARRAY,
        property_type_image2DArray = GL_IMAGE_2D_ARRAY,
        property_type_image2DMS = GL_IMAGE_2D_MULTISAMPLE,
        property_type_image2DMSArray = GL_IMAGE_2D_MULTISAMPLE_ARRAY,
        property_type_iimage1D = GL_INT_IMAGE_1D,
        property_type_iimage2D = GL_INT_IMAGE_2D,
        property_type_iimage3D = GL_INT_IMAGE_3D,
        property_type_iimage2DRect = GL_INT_IMAGE_2D_RECT,
        property_type_iimageCube = GL_INT_IMAGE_CUBE,
        property_type_iimageBuffer = GL_INT_IMAGE_BUFFER,
        property_type_iimage1DArray = GL_INT_IMAGE_1D_ARRAY,
        property_type_iimage2DArray = GL_INT_IMAGE_2D_ARRAY,
        property_type_iimage2DMS = GL_INT_IMAGE_2D_MULTISAMPLE,
        property_type_iimage2DMSArray = GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
        property_type_uimage1D = GL_UNSIGNED_INT_IMAGE_1D,
        property_type_uimage2D = GL_UNSIGNED_INT_IMAGE_2D,
        property_type_uimage3D = GL_UNSIGNED_INT_IMAGE_3D,
        property_type_uimage2DRect = GL_UNSIGNED_INT_IMAGE_2D_RECT,
        property_type_uimageCube = GL_UNSIGNED_INT_IMAGE_CUBE,
        property_type_uimageBuffer = GL_UNSIGNED_INT_IMAGE_BUFFER,
        property_type_uimage1DArray = GL_UNSIGNED_INT_IMAGE_1D_ARRAY,
        property_type_uimage2DArray = GL_UNSIGNED_INT_IMAGE_2D_ARRAY,
        property_type_uimage2DMS = GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,
        property_type_uimage2DMSArray =
            GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
        property_type_atomic_uint = GL_UNSIGNED_INT_ATOMIC_COUNTER
    };

    std::string _name;
    int _index;
    int _size;
    type _type;
    std::any _value;
};
