#ifndef __AETHER_RENDER__
#define __AETHER_RENDER__

#include "aether_tga.h"
#include "aether_model.h"

void line_ae_render(ae_vec2_i p1, ae_vec2_i p2, ae_tga_i *image, ae_tga_c *color);
void triangle_ae_render(ae_vec3_f p1, ae_vec3_f p2, ae_vec3_f p3, double *zbuffer, ae_tga_i *image, ae_tga_c *color);

void render_model(ae_model *model, ae_tga_i *image, ae_tga_c *color);

#endif // __AETHER_RENDER__
