#ifndef __AETHER_TGA_H__
#define __AETHER_TGA_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// header
#pragma pack(push, 1)
typedef struct ae_tga_h
{
	int8_t idlength;
	int8_t colormaptype;
	int8_t datatypecode;
	int16_t colormaporigin;
	int16_t colormaplength;
	int8_t colormapdepth;
	int16_t x_origin;
	int16_t y_origin;
	int16_t width;
	int16_t height;
	int8_t bitsperpixel;
	int8_t imagedescriptor;
} ae_tga_h;
#pragma pack(pop)

// color
typedef struct ae_tga_c
{
	union
	{
		struct
		{
			uint8_t b, g, r, a;
		};
		uint8_t raw[4];
		uint32_t val;
	};
	int32_t bytespp;
} ae_tga_c;

#define AE_TGA_C_DEF(color) ae_tga_c color = {.val = 0, .bytespp = 1};
#define AE_TGA_C_RGBA(color, R, G, B, A) ae_tga_c color = {.r = R, .g = G, .b = B, .a = A, .bytespp = 4};
#define AE_TGA_C_V(color, v, bpp) ae_tga_c color = {.val = v, .bytespp = bpp};
#define AE_TGA_C_Color(color, color_from) ae_tga_c color = {.val = color_from.val, .bytespp = color_from.bytespp};
#define AE_TGA_C_RAW(color, p, bpp)             \
	ae_tga_c color = {.bytespp = bpp};          \
	for (int32_t i = 0; i < bpp; i++)           \
	{                                           \
		color.raw[i] = ((unsigned char *)p)[i]; \
	}

// image
typedef struct ae_tga_i
{
	uint8_t *data;
	int32_t width;
	int32_t height;
	int32_t bytespp;
} ae_tga_i;

typedef enum
{
	GRAYSCALE = 1,
	RGB = 3,
	RGBA = 4
} AETHER_TGA_FORMAT;

ae_tga_i create_default_ae_tga(void);
ae_tga_i create_ae_tga(int32_t w, int32_t h, int32_t bpp);
ae_tga_i create_from_tga_ae_tga(const ae_tga_i *image);
bool load_rle_data_ae_tga(ae_tga_i *image, FILE *in);
bool unload_rle_data_ae_tga(ae_tga_i *image, FILE *out);
bool read_file_ae_tga(ae_tga_i *image, const char *filename);
bool write_file_ae_tga(ae_tga_i *image, const char *filename, bool rle);
bool flip_horizontally_ae_tga(ae_tga_i *image);
bool flip_vertically_ae_tga(ae_tga_i *image);
bool scale_ae_tga(ae_tga_i *image, int32_t w, int32_t h);
ae_tga_c get_ae_tga(ae_tga_i *image, int32_t x, int32_t y);
bool set_ae_tga(ae_tga_i *image, int32_t x, int32_t y, ae_tga_c *c);
void free_ae_tga(ae_tga_i *image);
void clear_ae_tga(ae_tga_i *image);

#endif //__AETHER_TGA_H__
