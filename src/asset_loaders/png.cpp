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

    auto width = png_get_image_width(png, info);
    auto height = png_get_image_height(png, info);
    auto color_type = png_get_color_type(png, info);
    auto bit_depth = png_get_bit_depth(png, info);

    // For simplicity let the modifications when loading png images
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    png_read_update_info(png, info);

    char* buffer = new char[ height * png_get_rowbytes(png, info) ];
    for (int i = 0; i < height; ++i)
    {
        png_read_row(png,
                     reinterpret_cast<png_bytep>(
                         buffer + i * png_get_rowbytes(png, info)),
                     nullptr);
    }

    fclose(fp);

    image::metadata md;

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    md._width = width;
    md._height = height;

    switch (color_type)
    /* Color type covnersion is done based on the following table
        0       1,2,4,8,16  Each pixel is a grayscale sample.
        2       8,16        Each pixel is an R,G,B triple.
        3       1,2,4,8     Each pixel is a palette index;
                            a PLTE chunk must appear.
        4       8,16        Each pixel is a grayscale sample,
                            followed by an alpha sample.
        6       8,16        Each pixel is an R,G,B triple,
                            followed by an alpha sample.
        http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html
    */
    {
    case 0:
    {
        md._color_type = image::color_type::GRAYSCALE;
        md._channel_count = 1;
        break;
    }
    case 2:
    {
        md._color_type = image::color_type::RGB;
        md._channel_count = 3;
        break;
    }
    case 3:
    {
        md._color_type = image::color_type::PALETTE;
        md._channel_count = 3;
        break;
    }
    case 4:
    {
        md._color_type = image::color_type::GRAYSCALE_ALPHA;
        md._channel_count = 2;
        break;
    }
    case 6:
    {
        md._color_type = image::color_type::RGBA;
        md._channel_count = 4;
        break;
    }
    default:
    {
        md._color_type = image::color_type::UNSPECIFIED;
        md._channel_count = 0;
        break;
    }
    }

    md._bits_per_pixel = bit_depth * md._channel_count;
    md._bytes_per_row = png_get_rowbytes(png, info);
    md._file_format = image::file_format::PNG;

    png_destroy_read_struct(&png, &info, NULL);

    _image = new image;
    _image->init(md);
    _image->set_data(reinterpret_cast<char*>(buffer));
}

void asset_loader_PNG::save(std::string_view path)
{
    FILE* fp = fopen(path.data(), "wb");
    if (!fp)
        abort();

    png_structp png =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
        abort();

    if (setjmp(png_jmpbuf(png)))
        abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(png,
                 info,
                 _image->get_width(),
                 _image->get_height(),
                 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    for (size_t i = _image->get_height(); i > 0; --i)
    {
        png_write_row(png,
                      _image->get_data<unsigned char>() +
                          (i - 1) * _image->get_metadata()._bytes_per_row);
    }

    png_write_end(png, NULL);
    fclose(fp);

    png_destroy_write_struct(&png, &info);
}

image* asset_loader_PNG::get_image() { return _image; }

void asset_loader_PNG::set_image(image* img) { _image = img; }
