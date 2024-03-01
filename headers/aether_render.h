// based on https://github.com/ssloy/tinyrenderer/wiki
#ifndef __AETHER_RENDER__
#define __AETHER_RENDER__

#include "aether_tga.h"
#include "aether_model.h"

typedef struct ae_render_data
{
    ae_vec3_f p[3];
    ae_vec2_f uvs[3];
    double intensity[3];
} ae_render_data;

void line_ae_render(ae_vec3_f p1, ae_vec3_f p2, ae_tga_i *image, ae_tga_c *color);
ae_vec3_f barycentric_ae_render(ae_vec3_f *A, ae_vec3_f *P);
void triangle_ae_render(ae_tga_i *image, ae_model *model, ae_render_data *data, double *zbuffer);

#define PI 3.14159265

#define AE_M2VI_RENDER(v, m)                                              \
    v.x = (int32_t)(AE_MATRIX_F_GET(m, 0, 0) / AE_MATRIX_F_GET(m, 3, 0)); \
    v.y = (int32_t)(AE_MATRIX_F_GET(m, 1, 0) / AE_MATRIX_F_GET(m, 3, 0)); \
    v.z = (int32_t)(AE_MATRIX_F_GET(m, 2, 0) / AE_MATRIX_F_GET(m, 3, 0));

#define AE_M2VF_RENDER(v, m)                                     \
    v.x = (AE_MATRIX_F_GET(m, 0, 0) / AE_MATRIX_F_GET(m, 3, 0)); \
    v.y = (AE_MATRIX_F_GET(m, 1, 0) / AE_MATRIX_F_GET(m, 3, 0)); \
    v.z = (AE_MATRIX_F_GET(m, 2, 0) / AE_MATRIX_F_GET(m, 3, 0));

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

#define AE_LOOK_AT(m, eye, center, up)                 \
    AE_MATRIX_F_IDENTITY(m, 4);                        \
    {                                                  \
        ae_vec3_f z;                                   \
        AE_VEC3_DIF(z, eye, center);                   \
        AE_VEC3_NORMALIZE(z, z, 1);                    \
        ae_vec3_f x;                                   \
        AE_VEC3_CROSS(x, up, z);                       \
        AE_VEC3_NORMALIZE(x, x, 1);                    \
        ae_vec3_f y;                                   \
        AE_VEC3_CROSS(y, z, x);                        \
        AE_VEC3_NORMALIZE(y, y, 1);                    \
        for (size_t i = 0; i < 3; i++)                 \
        {                                              \
            AE_MATRIX_F_GET(m, 0, i) = x.raw[i];       \
            AE_MATRIX_F_GET(m, 1, i) = y.raw[i];       \
            AE_MATRIX_F_GET(m, 2, i) = z.raw[i];       \
            AE_MATRIX_F_GET(m, i, 3) = -center.raw[i]; \
        }                                              \
    }

#define AE_DEFORM(vp, v)                      \
    {                                         \
        AE_V2M_RENDER(m_1, v);                \
        AE_MATRIX_F_MULT(m_2, vp, m_1, 4, 1); \
        AE_M2VI_RENDER(v, m_2);                \
    }

void render_model(ae_model *model, ae_tga_i *image);

#endif // __AETHER_RENDER__
