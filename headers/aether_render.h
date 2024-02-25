#ifndef __AETHER_RENDER__
#define __AETHER_RENDER__

#include "aether_tga.h"
#include "aether_model.h"

void line_ae_render(ae_vec3_f p1, ae_vec3_f p2, ae_tga_i *image, ae_tga_c *color);
void triangle_ae_render(ae_tga_i *image, ae_tga_i *texture, ae_vec3_f *p, ae_vec2_f *uv, double intensity, double *zbuffer);
ae_vec3_f barycentric_ae_render(ae_vec3_f A, ae_vec3_f B, ae_vec3_f C, ae_vec3_f P);

#define AE_M2V_RENDER(v, m)                                    \
    ae_vec3_f v;                                               \
    v.x = AE_MATRIX_F_GET(m, 0, 0) / AE_MATRIX_F_GET(m, 3, 0); \
    v.y = AE_MATRIX_F_GET(m, 1, 0) / AE_MATRIX_F_GET(m, 3, 0); \
    v.z = AE_MATRIX_F_GET(m, 2, 0) / AE_MATRIX_F_GET(m, 3, 0);

#define AE_V2M_RENDER(m, v)         \
    AE_MATRIX_F_CREATE(m, 4, 1);    \
    AE_MATRIX_F_GET(m, 0, 0) = v.x; \
    AE_MATRIX_F_GET(m, 1, 0) = v.y; \
    AE_MATRIX_F_GET(m, 2, 0) = v.z; \
    AE_MATRIX_F_GET(m, 3, 0) = 1.0;

#define AE_VIEWPORT_RENDER(m, depth, x, y, w, h) \
    AE_MATRIX_F_IDENTITY(m, 4);                  \
    AE_MATRIX_F_GET(m, 0, 3) = x + w / 2.0;      \
    AE_MATRIX_F_GET(m, 1, 3) = y + h / 2.0;      \
    AE_MATRIX_F_GET(m, 2, 3) = depth / 2.0;      \
    AE_MATRIX_F_GET(m, 0, 0) = w / 2.0;          \
    AE_MATRIX_F_GET(m, 1, 1) = h / 2.0;          \
    AE_MATRIX_F_GET(m, 2, 2) = depth / 2.0;

#define AE_TRANSLATION_RENDER(m, v) \
    AE_MATRIX_F_IDENTITY(m, 4);     \
    AE_MATRIX_F_GET(m, 0, 3) = v.x; \
    AE_MATRIX_F_GET(m, 1, 3) = v.y; \
    AE_MATRIX_F_GET(m, 2, 3) = v.z;

#define AE_ZOOM_RENDER(m, f)    \
    AE_MATRIX_F_IDENTITY(m, 4); \
    AE_MATRIX_F_GET(m, 0, 0) = AE_MATRIX_F_GET(m, 1, 1) = AE_MATRIX_F_GET(m, 2, 2) = f;

#define AE_ROTATION_X_RENDER(m, c, s)                        \
    AE_MATRIX_F_IDENTITY(m, 4);                              \
    AE_MATRIX_F_GET(m, 1, 1) = AE_MATRIX_F_GET(m, 2, 2) = c; \
    AE_MATRIX_F_GET(m, 1, 2) = -s;                           \
    AE_MATRIX_F_GET(m, 2, 1) = s;

#define AE_ROTATION_Y_RENDER(m, c, s)                        \
    AE_MATRIX_F_IDENTITY(m, 4);                              \
    AE_MATRIX_F_GET(m, 0, 0) = AE_MATRIX_F_GET(m, 2, 2) = c; \
    AE_MATRIX_F_GET(m, 0, 2) = s;                            \
    AE_MATRIX_F_GET(m, 2, 0) = -s;

#define AE_ROTATION_Z_RENDER(m, c, s)                        \
    AE_MATRIX_F_IDENTITY(m, 4);                              \
    AE_MATRIX_F_GET(m, 0, 0) = AE_MATRIX_F_GET(m, 1, 1) = c; \
    AE_MATRIX_F_GET(m, 0, 1) = -s;                           \
    AE_MATRIX_F_GET(m, 1, 0) = s;

void render_model(ae_model *model, ae_tga_i *image);

#endif // __AETHER_RENDER__
