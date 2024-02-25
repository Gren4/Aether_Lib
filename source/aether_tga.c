#include "aether_tga.h"
#include <string.h>
#include <stdlib.h>

ae_tga_i create_default_ae_tga(void)
{
    ae_tga_i new_image = {
        .data = NULL,
        .width = 0,
        .height = 0,
        .bytespp = 0};

    return new_image;
}

ae_tga_i create_ae_tga(int32_t w, int32_t h, int32_t bpp)
{
    size_t nbytes = w * h * bpp;
    ae_tga_i new_image = {
        .data = malloc(sizeof(uint8_t) * nbytes),
        .width = w,
        .height = h,
        .bytespp = bpp};
    if (new_image.data != NULL)
        memset(new_image.data, 0, nbytes);

    return new_image;
}

ae_tga_i create_from_tga_ae_tga(const ae_tga_i *image)
{
    size_t nbytes = image->width * image->height * image->bytespp;
    ae_tga_i new_image = {
        .data = malloc(sizeof(uint8_t) * nbytes),
        .width = image->width,
        .height = image->height,
        .bytespp = image->bytespp};

    memcpy(new_image.data, image->data, nbytes);

    return new_image;
}

void free_ae_tga(ae_tga_i *image)
{
    if (image->data != NULL)
    {
        free(image->data);
        image->data = NULL;
    }

    return;
}

bool read_file_ae_tga(ae_tga_i *image, const char *filename)
{
    free_ae_tga(image);

    FILE *in;
    if ((in = fopen(filename, "rb")) == NULL)
    {
        fclose(in);
        return false;
    }

    char header[18];
    if (fread(header, 1, sizeof(header), in) != sizeof(header))
    {
        fclose(in);
        return false;
    }

    image->width = header[13] << 8 | header[12];
    image->height = header[15] << 8 | header[14];
    image->bytespp = header[16] >> 3;

    if (image->width <= 0 || image->height <= 0 || (image->bytespp != GRAYSCALE && image->bytespp != RGB && image->bytespp != RGBA))
    {
        fclose(in);
        return false;
    }

    size_t nbytes = image->bytespp * image->width * image->height;
    image->data = malloc(sizeof(uint8_t) * nbytes);

    switch (header[2])
    {
    case 3:
    case 2:
        if (fread((char *)image->data, 1, nbytes, in) != nbytes)
        {
            fclose(in);
            return false;
        }
        break;
    case 10:
    case 11:
        if (!load_rle_data_ae_tga(image, in))
        {
            fclose(in);
            return false;
        }
        break;

    default:
        fclose(in);
        return false;
        break;
    }

    if (!(header[17] & 0x20))
    {
        flip_vertically_ae_tga(image);
    }

    if (!(header[17] & 0x10))
    {
        flip_horizontally_ae_tga(image);
    }

    fclose(in);

    return true;
}

bool load_rle_data_ae_tga(ae_tga_i *image, FILE *in)
{
    size_t pixelcount = image->width * image->height;
    size_t currentpixel = 0;
    size_t currentbyte = 0;

    AE_TGA_C_DEF(colorbuffer);

    do
    {
        uint8_t chunkheader = 0;
        if ((chunkheader = fgetc(in)) == EOF)
        {
            return false;
        }
        if (chunkheader < 128)
        {
            chunkheader++;
            for (size_t i = 0; i < chunkheader; i++)
            {
                if (fread((char *)colorbuffer.raw, 1, image->bytespp, in) != image->bytespp)
                {
                    return false;
                }
                for (size_t t = 0; t < image->bytespp; t++)
                {
                    image->data[currentbyte++] = colorbuffer.raw[t];
                }
                currentpixel++;
                if (currentpixel > pixelcount)
                {
                    return false;
                }
            }
        }
        else
        {
            chunkheader -= 127;
            if (fread((char *)colorbuffer.raw, 1, image->bytespp, in) != image->bytespp)
            {
                return false;
            }
            for (size_t i = 0; i < chunkheader; i++)
            {
                for (size_t t = 0; t < image->bytespp; t++)
                {
                    image->data[currentbyte++] = colorbuffer.raw[t];
                }
                currentpixel++;
                if (currentpixel > pixelcount)
                {
                    return false;
                }
            }
        }
    } while (currentpixel < pixelcount);

    return true;
}

bool write_file_ae_tga(ae_tga_i *image, const char *filename, bool rle)
{
    uint8_t developer_area_ref[4] = {0, 0, 0, 0};
    uint8_t extension_area_ref[4] = {0, 0, 0, 0};
    uint8_t footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};

    FILE *out;
    if ((out = fopen(filename, "wb")) == NULL)
    {
        fclose(out);
        return false;
    }

    char header[18] = {0};
    header[16] = image->bytespp << 3;
    header[12] = image->width & 0xff;
    header[13] = image->width >> 8;
    header[14] = image->height & 0xff;
    header[15] = image->height >> 8;
    header[2] = (image->bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header[17] = 0x20;
    if (fwrite(header, 1, sizeof(header), out) != sizeof(header))
    {
        fclose(out);
        return false;
    }
    if (!rle)
    {
        if (fwrite((char *)image->data, 1, image->width * image->height * image->bytespp, out) != image->width * image->height * image->bytespp)
        {
            fclose(out);
            return false;
        }
    }
    else
    {
        if (!unload_rle_data_ae_tga(image, out))
        {
            fclose(out);
            return false;
        }
    }

    if (fwrite((char *)developer_area_ref, 1, sizeof(developer_area_ref), out) != sizeof(developer_area_ref))
    {
        fclose(out);
        return false;
    }
    if (fwrite((char *)extension_area_ref, 1, sizeof(extension_area_ref), out) != sizeof(extension_area_ref))
    {
        fclose(out);
        return false;
    }
    if (fwrite((char *)footer, 1, sizeof(footer), out) != sizeof(footer))
    {
        fclose(out);
        return false;
    }

    fclose(out);

    return true;
}

bool unload_rle_data_ae_tga(ae_tga_i *image, FILE *out)
{
    const uint8_t max_chunk_length = 128;
    size_t npixels = image->width * image->height;
    size_t curpix = 0;
    size_t chunkstart;
    size_t curbyte;
    uint8_t run_length;
    while (curpix < npixels)
    {
        chunkstart = curpix * image->bytespp;
        curbyte = chunkstart;
        run_length = 1;
        bool raw = true;
        while (curpix + run_length < npixels && run_length < max_chunk_length)
        {
            bool succ_eq = true;
            for (size_t t = 0; succ_eq && t < image->bytespp; t++)
            {
                succ_eq = (image->data[curbyte + t] == image->data[curbyte + t + image->bytespp]);
            }
            curbyte += image->bytespp;
            if (run_length == 1)
            {
                raw = !succ_eq;
            }
            if (raw && succ_eq)
            {
                run_length--;
                break;
            }
            if (!raw && !succ_eq)
            {
                break;
            }
            run_length++;
        }
        curpix += run_length;
        if (fputc(raw ? run_length - 1 : run_length + 127, out) == EOF)
        {
            return false;
        }
        if (fwrite((char *)(image->data + chunkstart), 1, (raw ? run_length * image->bytespp : image->bytespp), out) != (raw ? run_length * image->bytespp : image->bytespp))
        {
            return false;
        }
    }

    return true;
}

ae_tga_c get_ae_tga(ae_tga_i *image, int32_t x, int32_t y)
{
    if (image->data == NULL || x < 0 || y < 0 || x >= image->width || y >= image->height)
    {
        AE_TGA_C_DEF(color);
        return color;
    }
    else
    {
        AE_TGA_C_RAW(color, (image->data + (x + y * image->width) * image->bytespp), image->bytespp);
        return color;
    }
}

bool set_ae_tga(ae_tga_i *image, int32_t x, int32_t y, ae_tga_c *c)
{
    if (image->data == NULL || x < 0 || y < 0 || x >= image->width || y >= image->height)
    {
        return false;
    }
    else
    {
        memcpy(image->data + (x + y * image->width) * image->bytespp, c->raw, image->bytespp);
        return true;
    }
}

bool flip_horizontally_ae_tga(ae_tga_i *image)
{
    if (image->data == NULL)
        return false;
    size_t half = image->width >> 1;
    ae_tga_c c1;
    ae_tga_c c2;
    for (size_t i = 0; i < half; i++)
    {
        for (size_t j = 0; j < image->height; j++)
        {
            c1 = get_ae_tga(image, i, j);
            c2 = get_ae_tga(image, image->width - 1 - i, j);
            set_ae_tga(image, i, j, &c2);
            set_ae_tga(image, image->width - 1 - i, j, &c1);
        }
    }

    return true;
}
bool flip_vertically_ae_tga(ae_tga_i *image)
{
    if (image->data == NULL)
        return false;
    size_t bytes_per_line = image->width * image->bytespp;
    uint8_t *line = malloc(bytes_per_line);
    size_t half = image->height >> 1;
    size_t l1;
    size_t l2;
    for (size_t j = 0; j < half; j++)
    {
        l1 = j * bytes_per_line;
        l2 = (image->height - 1 - j) * bytes_per_line;
        memmove((void *)line, (void *)(image->data + l1), bytes_per_line);
        memmove((void *)(image->data + l1), (void *)(image->data + l2), bytes_per_line);
        memmove((void *)(image->data + l2), (void *)line, bytes_per_line);
    }
    free(line);

    return true;
}

void clear_ae_tga(ae_tga_i *image)
{
    memset((void *)image->data, 0, image->width * image->height * image->bytespp);
    return;
}

bool scale_ae_tga(ae_tga_i *image, int32_t w, int32_t h)
{
    if (w <= 0 || h <= 0 || image->data == NULL)
        return false;
    uint8_t *tdata = malloc(w * h * image->bytespp);
    size_t nscanline = 0;
    size_t oscanline = 0;
    size_t erry = 0;
    size_t nlinebytes = w * image->bytespp;
    size_t olinebytes = image->width * image->bytespp;
    size_t errx;
    size_t nx;
    size_t ox;
    for (size_t j = 0; j < image->height; j++)
    {
        errx = image->width - w;
        nx = -image->bytespp;
        ox = nx;
        for (size_t i = 0; i < image->width; i++)
        {
            ox += image->bytespp;
            errx += w;
            while (errx >= (size_t)image->width)
            {
                errx -= image->width;
                nx += image->bytespp;
                memcpy(tdata + nscanline + nx, image->data + oscanline + ox, image->bytespp);
            }
        }
        erry += h;
        oscanline += olinebytes;
        while (erry >= (size_t)image->height)
        {
            if (erry >= (size_t)image << 1)
                memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
            erry -= image->height;
            nscanline += nlinebytes;
        }
    }
    free(image->data);
    image->data = tdata;
    image->width = w;
    image->height = h;

    return true;
}
