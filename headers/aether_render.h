#ifndef __AETHER_RENDER__
#define __AETHER_RENDER__

#include "aether_tga.h"
#include "aether_model.h"

void line_ae_render(ae_vec2_i p1, ae_vec2_i p2, ae_tga_i *image, ae_tga_c *color);
void triangle_ae_render(ae_tga_i *image, ae_tga_i *texture, ae_vec3_f *p, ae_vec2_f *uv, double intensity, double *zbuffer);
ae_vec3_f barycentric_ae_render(ae_vec3_f A, ae_vec3_f B, ae_vec3_f C, ae_vec3_f P);

void render_model(ae_model *model, ae_tga_i *image);

#endif // __AETHER_RENDER__
