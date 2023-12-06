#include <png.h>

#include "asset_loaders/png.hpp"

#include "image.hpp"

void asset_loader_PNG::load(std::string_view path)
{
    FILE* fp = fopen(path.data(), "rb");

    png_structp png =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
        abort();

    if (setjmp(png_jmpbuf(png)))
        abort();

    png_init_io(png, fp);

    png_read_info(png, info);

    _image = new image;
    auto width = png_get_image_width(png, info);
    auto height = png_get_image_height(png, info);
    _image->init(width, height);
    auto color_type = png_get_color_type(png, info);
    auto bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    // png_bytep* row_pointers = NULL;
    // row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    // for (int y = 0; y < height; y++)
    // {
    //     row_pointers[ y ] = (png_byte*)malloc(png_get_rowbytes(png, info));
    // }

    char* buffer = new char[ height * png_get_rowbytes(png, info) ];
    for (int i = 0; i < height; ++i)
    {
        png_read_row(png, reinterpret_cast<png_bytep>(buffer + i * png_get_rowbytes(png, info)), nullptr);
    }
    // png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);
    _image->set_data(reinterpret_cast<char*>(buffer));
}

image* asset_loader_PNG::get_image() { return _image; }
