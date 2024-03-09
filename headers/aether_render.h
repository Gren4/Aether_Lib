// based on https://github.com/ssloy/tinyrenderer/wiki
#ifndef __AETHER_RENDER__
#define __AETHER_RENDER__

#include "aether_tga.h"
#include "aether_model.h"
#include "aether_shaders.h"

void line_ae_render(ae_vec3_f p1, ae_vec3_f p2, ae_tga_i *image, ae_tga_c const *color);
ae_vec3_f barycentric_ae_render(ae_vec3_f const *A, ae_vec3_f *P);
void triangle_ae_render(ae_tga_i *image, ae_model const *model, ae_shader const *shader, double *zbuffer);
void shadow_buffer_ae_render(ae_tga_i *image, ae_model const *model, ae_shader const *shader, double *zbuffer);
void render_model(ae_vec const *models);

#endif // __AETHER_RENDER__
