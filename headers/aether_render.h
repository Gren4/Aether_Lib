#ifndef __AETHER_RENDER__
#define __AETHER_RENDER__

#include "aether_tga.h"
#include "aether_model.h"

void line_ae_render(int32_t x0, int32_t y0, int32_t x1, int32_t y1, ae_tga_i *image, ae_tga_c *color);
void render_model(ae_model *model, ae_tga_i *image, ae_tga_c * color);

#endif // __AETHER_RENDER__
