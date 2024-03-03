// based on https://github.com/ssloy/tinyrenderer/wiki
#ifndef __AETHER_RENDER_GV_H__
#define __AETHER_RENDER_GV_H__

#include "aether_geometry.h"

extern AE_MATRIX_F_DECLARE(ViewPort, 4, 4);
extern AE_MATRIX_F_DECLARE(ModelView, 4, 4);
extern AE_MATRIX_F_DECLARE(Projection, 4, 4);
extern AE_MATRIX_F_DECLARE(Proj_ModelView, 4, 4);
extern AE_MATRIX_F_DECLARE(Proj_ModelView_IT, 4, 4);
extern AE_MATRIX_F_DECLARE(Z, 4, 4);
extern AE_MATRIX_F_DECLARE(Z_shdw, 4, 4);
extern AE_MATRIX_F_DECLARE(Shadow, 4, 4);
extern AE_MATRIX_F_DECLARE(r_xyz, 4, 4);

extern ae_vec3_f light_dir;
extern ae_vec3_f l;
extern ae_vec3_f eye;
extern ae_vec3_f center;
extern ae_vec3_f up;

extern double eye_center_dif;
extern double *shadow_buffer;

extern int32_t width;
extern int32_t height;
extern int32_t depth;

#define PI 3.14159265

#define AE_M_x_V_F_EMBED_RENDER(result, m, v)                                                                                                                               \
    {                                                                                                                                                                       \
        ae_vec3_f temp = {.x = v.x, .y = v.y, .z = v.z};                                                                                                                    \
        double coeff = (AE_MATRIX_F_GET(m, 3, 0) * temp.x + AE_MATRIX_F_GET(m, 3, 1) * temp.y + AE_MATRIX_F_GET(m, 3, 2) * temp.z + AE_MATRIX_F_GET(m, 3, 3));              \
        for (size_t i = 0; i < 3; i++)                                                                                                                                      \
            result.raw[i] = (AE_MATRIX_F_GET(m, i, 0) * temp.x + AE_MATRIX_F_GET(m, i, 1) * temp.y + AE_MATRIX_F_GET(m, i, 2) * temp.z + AE_MATRIX_F_GET(m, i, 3)) / coeff; \
    }

#define AE_M_x_V_I_EMBED_RENDER(result, m, v)                                                                                                                                          \
    {                                                                                                                                                                                  \
        ae_vec3_f temp = {.x = v.x, .y = v.y, .z = v.z};                                                                                                                               \
        double coeff = (AE_MATRIX_F_GET(m, 3, 0) * temp.x + AE_MATRIX_F_GET(m, 3, 1) * temp.y + AE_MATRIX_F_GET(m, 3, 2) * temp.z + AE_MATRIX_F_GET(m, 3, 3));                         \
        for (size_t i = 0; i < 3; i++)                                                                                                                                                 \
            result.raw[i] = (int32_t)((AE_MATRIX_F_GET(m, i, 0) * temp.x + AE_MATRIX_F_GET(m, i, 1) * temp.y + AE_MATRIX_F_GET(m, i, 2) * temp.z + AE_MATRIX_F_GET(m, i, 3)) / coeff); \
    }

#define AE_M_x_V_F_PROJ_RENDER(result, m, v, fill)                                                                                                                         \
    {                                                                                                                                                                      \
        ae_vec3_f temp = {.x = v.x, .y = v.y, .z = v.z};                                                                                                                   \
        for (size_t i = 0; i < 3; i++)                                                                                                                                     \
            result.raw[i] = (AE_MATRIX_F_GET(m, i, 0) * temp.x + AE_MATRIX_F_GET(m, i, 1) * temp.y + AE_MATRIX_F_GET(m, i, 2) * temp.z + AE_MATRIX_F_GET(m, i, 3) * fill); \
    }

#define AE_M_x_V_I_PROJ_RENDER(result, m, v, fill)                                                                                                                                    \
    {                                                                                                                                                                                 \
        ae_vec3_f temp = {.x = v.x, .y = v.y, .z = v.z};                                                                                                                              \
        for (size_t i = 0; i < 3; i++)                                                                                                                                                \
            result.raw[i] = (int32_t)((AE_MATRIX_F_GET(m, i, 0) * temp.x + AE_MATRIX_F_GET(m, i, 1) * temp.y + AE_MATRIX_F_GET(m, i, 2) * temp.z + AE_MATRIX_F_GET(m, i, 3) * fill)); \
    }

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
    AE_MATRIX_F_GET(m, 0, 0) = w / 2.0;          \
    AE_MATRIX_F_GET(m, 0, 1) = 0.0;              \
    AE_MATRIX_F_GET(m, 0, 2) = 0.0;              \
    AE_MATRIX_F_GET(m, 0, 3) = x + w / 2.0;      \
    AE_MATRIX_F_GET(m, 1, 0) = 0.0;              \
    AE_MATRIX_F_GET(m, 1, 1) = h / 2.0;          \
    AE_MATRIX_F_GET(m, 1, 2) = 0.0;              \
    AE_MATRIX_F_GET(m, 1, 3) = y + h / 2.0;      \
    AE_MATRIX_F_GET(m, 2, 0) = 0.0;              \
    AE_MATRIX_F_GET(m, 2, 1) = 0.0;              \
    AE_MATRIX_F_GET(m, 2, 2) = depth / 2.0;      \
    AE_MATRIX_F_GET(m, 2, 3) = depth / 2.0;      \
    AE_MATRIX_F_GET(m, 3, 0) = 0.0;              \
    AE_MATRIX_F_GET(m, 3, 1) = 0.0;              \
    AE_MATRIX_F_GET(m, 3, 2) = 0.0;              \
    AE_MATRIX_F_GET(m, 3, 3) = 1.0;

#define AE_PROJECTION_RENDER(m, coeff) \
    AE_MATRIX_F_GET(m, 0, 0) = 1.0;    \
    AE_MATRIX_F_GET(m, 0, 1) = 0.0;    \
    AE_MATRIX_F_GET(m, 0, 2) = 0.0;    \
    AE_MATRIX_F_GET(m, 0, 3) = 0.0;    \
    AE_MATRIX_F_GET(m, 1, 0) = 0.0;    \
    AE_MATRIX_F_GET(m, 1, 1) = 1.0;    \
    AE_MATRIX_F_GET(m, 1, 2) = 0.0;    \
    AE_MATRIX_F_GET(m, 1, 3) = 0.0;    \
    AE_MATRIX_F_GET(m, 2, 0) = 0.0;    \
    AE_MATRIX_F_GET(m, 2, 1) = 0.0;    \
    AE_MATRIX_F_GET(m, 2, 2) = 1.0;    \
    AE_MATRIX_F_GET(m, 2, 3) = 0.0;    \
    AE_MATRIX_F_GET(m, 3, 0) = 0.0;    \
    AE_MATRIX_F_GET(m, 3, 1) = 0.0;    \
    AE_MATRIX_F_GET(m, 3, 2) = coeff;  \
    AE_MATRIX_F_GET(m, 3, 3) = 1.0;

#define AE_TRANSLATION_RENDER(m, v) \
    AE_MATRIX_F_GET(m, 0, 0) = 1.0; \
    AE_MATRIX_F_GET(m, 0, 1) = 0.0; \
    AE_MATRIX_F_GET(m, 0, 2) = 0.0; \
    AE_MATRIX_F_GET(m, 0, 3) = v.x; \
    AE_MATRIX_F_GET(m, 1, 0) = 0.0; \
    AE_MATRIX_F_GET(m, 1, 1) = 1.0; \
    AE_MATRIX_F_GET(m, 1, 2) = 0.0; \
    AE_MATRIX_F_GET(m, 1, 3) = v.y; \
    AE_MATRIX_F_GET(m, 2, 0) = 0.0; \
    AE_MATRIX_F_GET(m, 2, 1) = 0.0; \
    AE_MATRIX_F_GET(m, 2, 2) = 1.0; \
    AE_MATRIX_F_GET(m, 2, 3) = v.z; \
    AE_MATRIX_F_GET(m, 3, 0) = 0.0; \
    AE_MATRIX_F_GET(m, 3, 1) = 0.0; \
    AE_MATRIX_F_GET(m, 3, 2) = 0.0; \
    AE_MATRIX_F_GET(m, 3, 3) = 1.0;

#define AE_ZOOM_RENDER(m, f)        \
    AE_MATRIX_F_GET(m, 0, 0) = f;   \
    AE_MATRIX_F_GET(m, 0, 1) = 0.0; \
    AE_MATRIX_F_GET(m, 0, 2) = 0.0; \
    AE_MATRIX_F_GET(m, 0, 3) = 0.0; \
    AE_MATRIX_F_GET(m, 1, 0) = 0.0; \
    AE_MATRIX_F_GET(m, 1, 1) = f;   \
    AE_MATRIX_F_GET(m, 1, 2) = 0.0; \
    AE_MATRIX_F_GET(m, 1, 3) = 0.0; \
    AE_MATRIX_F_GET(m, 2, 0) = 0.0; \
    AE_MATRIX_F_GET(m, 2, 1) = 0.0; \
    AE_MATRIX_F_GET(m, 2, 2) = f;   \
    AE_MATRIX_F_GET(m, 2, 3) = 0.0; \
    AE_MATRIX_F_GET(m, 3, 0) = 0.0; \
    AE_MATRIX_F_GET(m, 3, 1) = 0.0; \
    AE_MATRIX_F_GET(m, 3, 2) = 0.0; \
    AE_MATRIX_F_GET(m, 3, 3) = 1.0;

#define AE_ROTATION_X_RENDER(m, c, s) \
    AE_MATRIX_F_CREATE(m, 4, 4);      \
    AE_MATRIX_F_GET(m, 0, 0) = 1.0;   \
    AE_MATRIX_F_GET(m, 0, 1) = 0.0;   \
    AE_MATRIX_F_GET(m, 0, 2) = 0.0;   \
    AE_MATRIX_F_GET(m, 0, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 1, 0) = 0.0;   \
    AE_MATRIX_F_GET(m, 1, 1) = c;     \
    AE_MATRIX_F_GET(m, 1, 2) = -s;    \
    AE_MATRIX_F_GET(m, 1, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 2, 0) = 0.0;   \
    AE_MATRIX_F_GET(m, 2, 1) = s;     \
    AE_MATRIX_F_GET(m, 2, 2) = c;     \
    AE_MATRIX_F_GET(m, 2, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 0) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 1) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 2) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 3) = 1.0;

#define AE_ROTATION_Y_RENDER(m, c, s) \
    AE_MATRIX_F_CREATE(m, 4, 4);      \
    AE_MATRIX_F_GET(m, 0, 0) = c;     \
    AE_MATRIX_F_GET(m, 0, 1) = 0.0;   \
    AE_MATRIX_F_GET(m, 0, 2) = s;     \
    AE_MATRIX_F_GET(m, 0, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 1, 0) = 0.0;   \
    AE_MATRIX_F_GET(m, 1, 1) = 1.0;   \
    AE_MATRIX_F_GET(m, 1, 2) = 0.0;   \
    AE_MATRIX_F_GET(m, 1, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 2, 0) = -s;    \
    AE_MATRIX_F_GET(m, 2, 1) = 0.0;   \
    AE_MATRIX_F_GET(m, 2, 2) = c;     \
    AE_MATRIX_F_GET(m, 2, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 0) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 1) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 2) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 3) = 1.0;

#define AE_ROTATION_Z_RENDER(m, c, s) \
    AE_MATRIX_F_CREATE(m, 4, 4);      \
    AE_MATRIX_F_GET(m, 0, 0) = c;     \
    AE_MATRIX_F_GET(m, 0, 1) = -s;    \
    AE_MATRIX_F_GET(m, 0, 2) = 0.0;   \
    AE_MATRIX_F_GET(m, 0, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 1, 0) = s;     \
    AE_MATRIX_F_GET(m, 1, 1) = c;     \
    AE_MATRIX_F_GET(m, 1, 2) = 0.0;   \
    AE_MATRIX_F_GET(m, 1, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 2, 0) = 0.0;   \
    AE_MATRIX_F_GET(m, 2, 1) = 0.0;   \
    AE_MATRIX_F_GET(m, 2, 2) = 1.0;   \
    AE_MATRIX_F_GET(m, 2, 3) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 0) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 1) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 2) = 0.0;   \
    AE_MATRIX_F_GET(m, 3, 3) = 1.0;

#define AE_LOOK_AT_RENDER(m, eye, center, up)           \
    {                                                   \
        ae_vec3_f z;                                    \
        AE_VEC3_DIF(z, eye, center);                    \
        eye_center_dif = -1.0 / AE_VEC3_NORM(z);        \
        AE_VEC3_NORMALIZE(z, z, 1);                     \
        ae_vec3_f x;                                    \
        AE_VEC3_CROSS(x, up, z);                        \
        AE_VEC3_NORMALIZE(x, x, 1);                     \
        ae_vec3_f y;                                    \
        AE_VEC3_CROSS(y, z, x);                         \
        AE_VEC3_NORMALIZE(y, y, 1);                     \
        AE_MATRIX_F_IDENTITY_CREATE(Minv, 4);           \
        AE_MATRIX_F_IDENTITY_CREATE(Tr, 4);             \
        for (size_t i = 0; i < 3; i++)                  \
        {                                               \
            AE_MATRIX_F_GET(Minv, 0, i) = x.raw[i];     \
            AE_MATRIX_F_GET(Minv, 1, i) = y.raw[i];     \
            AE_MATRIX_F_GET(Minv, 2, i) = z.raw[i];     \
            AE_MATRIX_F_GET(Tr, i, 3) = -center.raw[i]; \
        }                                               \
        AE_MATRIX_F_MULT(m, Minv, Tr, 4, 4);            \
    }

#define AE_SHADOW_M_RENDER(m)                     \
    {                                             \
        AE_MATRIX_F_INVERSE_CREATE(Z_i, Z, 4, 4); \
        AE_MATRIX_F_MULT(m, Z_shdw, Z_i, 4, 4);   \
    }

#define AE_R_XYZ_RENDER(m, x, y, z)                                    \
    {                                                                  \
        AE_ROTATION_X_RENDER(r_x, cos(x *PI / 180), sin(x *PI / 180)); \
        AE_ROTATION_Y_RENDER(r_y, cos(y *PI / 180), sin(y *PI / 180)); \
        AE_ROTATION_Z_RENDER(r_z, cos(z *PI / 180), sin(z *PI / 180)); \
        AE_MATRIX_F_MULT_CREATE(r_xy, r_x, r_y, 4, 4);                 \
        AE_MATRIX_F_MULT(m, r_xy, r_z, 4, 4);                          \
    }

#define AE_DEFORM(vp, v)                             \
    {                                                \
        AE_V2M_RENDER(m_1, v);                       \
        AE_MATRIX_F_MULT_CREATE(m_2, vp, m_1, 4, 1); \
        AE_M2VI_RENDER(v, m_2);                      \
    }

#endif //__AETHER_RENDER_GV_H__
