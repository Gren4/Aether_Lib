// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_render.h"
#include "aether_render_gv.h"
#include "aether_utils.h"
#include <math.h>
#include <float.h>

void line_ae_render(ae_vec3_f p1, ae_vec3_f p2, ae_tga_i *image, ae_tga_c const *color)
{
    bool steep = false;
    if (fabs(p1.x - p2.x) < fabs(p1.y - p2.y))
    {
        swap_ae(double, p1.x, p1.y);
        swap_ae(double, p2.x, p2.y);
        steep = true;
    }
    if (p1.x > p2.x)
    {
        swap_ae(ae_vec3_f, p1, p2);
    }
    int32_t dx = p2.x - p1.x;
    int32_t dy = p2.y - p1.y;
    int32_t derror2 = abs(dy) * 2;
    int32_t error2 = 0;
    int32_t y = p1.y;
    int32_t incr = (p2.y > p1.y) ? 1 : -1;
    if (steep)
    {
        for (int32_t x = p1.x; x <= p2.x; x++)
        {
            set_ae_tga(image, y, x, color);
            error2 += derror2;
            if (error2 > dx)
            {
                y += incr;
                error2 -= dx * 2;
            }
        }
    }
    else
    {
        for (int32_t x = p1.x; x <= p2.x; x++)
        {
            set_ae_tga(image, x, y, color);
            error2 += derror2;
            if (error2 > dx)
            {
                y += incr;
                error2 -= dx * 2;
            }
        }
    }

    return;
}

ae_vec3_f barycentric_ae_render(ae_vec3_f const *A, ae_vec3_f *P)
{
    ae_vec3_f s[2];
    for (size_t i = 0; i < 2; i++)
    {
        s[i].raw[0] = A[2].raw[i] - A[0].raw[i];
        s[i].raw[1] = A[1].raw[i] - A[0].raw[i];
        s[i].raw[2] = A[0].raw[i] - P->raw[i];
    }
    ae_vec3_f u = {.x = -1.0, .y = -1.0, .z = -1.0};
    ae_vec3_f ret = {.x = -1.0, .y = -1.0, .z = -1.0};
    AE_VEC3_CROSS(u, s[0], s[1]);
    if (!(fabs(u.raw[2]) < 0.0))
    {
        ret.x = 1.0 - (u.x + u.y) / u.z;
        ret.y = u.y / u.z;
        ret.z = u.x / u.z;
    }
    P->z = A[0].z * ret.x + A[1].z * ret.y + A[2].z * ret.z;
    return ret;
}

#define AE_TRIANGLE_RENDER_CYCLE(n1, n2)                                                                                                                  \
    {                                                                                                                                                     \
        if (point##n2.y >= 0)                                                                                                                             \
            for (size_t i = (point##n1.y >= 0 ? point##n1.y : 0); i <= ((point##n2.y < image->height) ? point##n2.y : image->height - 1); i++)            \
            {                                                                                                                                             \
                alpha = (double)(i - point0.y) / total_height;                                                                                            \
                beta = (double)(i - point##n1.y) / segment_height;                                                                                        \
                                                                                                                                                          \
                P.y = i;                                                                                                                                  \
                                                                                                                                                          \
                A = point0.x + (point2.x - point0.x) * alpha;                                                                                             \
                B = point##n1.x + (point##n2.x - point##n1.x) * beta;                                                                                     \
                                                                                                                                                          \
                if (A > B)                                                                                                                                \
                {                                                                                                                                         \
                    swap_ae(double, A, B);                                                                                                                \
                }                                                                                                                                         \
                if (B >= 0)                                                                                                                               \
                {                                                                                                                                         \
                    for (size_t j = (int32_t)A >= 0 ? (int32_t)A : 0; j <= (((int32_t)B < image->width) ? (int32_t)B : image->width - 1); j++)            \
                    {                                                                                                                                     \
                        P.x = j;                                                                                                                          \
                        bc_screen = barycentric_ae_render(data->p, &P);                                                                                   \
                        zbuffer_idx = (int32_t)P.x + (int32_t)P.y * image->width;                                                                         \
                        if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)                                                                        \
                            continue;                                                                                                                     \
                        if (zbuffer_idx < image->width * image->height && zbuffer[zbuffer_idx] < P.z)                                                     \
                        {                                                                                                                                 \
                            uv.u = bc_screen.raw[0] * data->uvs[0].u + bc_screen.raw[1] * data->uvs[1].u + bc_screen.raw[2] * data->uvs[2].u;             \
                            uv.v = bc_screen.raw[0] * data->uvs[0].v + bc_screen.raw[1] * data->uvs[1].v + bc_screen.raw[2] * data->uvs[2].v;             \
                                                                                                                                                          \
                            bn.x = bc_screen.raw[0] * data->normals[0].x + bc_screen.raw[1] * data->normals[1].x + bc_screen.raw[2] * data->normals[2].x; \
                            bn.y = bc_screen.raw[0] * data->normals[0].y + bc_screen.raw[1] * data->normals[1].y + bc_screen.raw[2] * data->normals[2].y; \
                            bn.z = bc_screen.raw[0] * data->normals[0].z + bc_screen.raw[1] * data->normals[1].z + bc_screen.raw[2] * data->normals[2].z; \
                                                                                                                                                          \
                            AE_VEC3_NORMALIZE(bn, bn, 1);                                                                                                 \
                                                                                                                                                          \
                            AE_MATRIX_F_SET_ROW_FROM_V_DIF(Am, 0, data->orig_p[1], data->orig_p[0]);                                                      \
                            AE_MATRIX_F_SET_ROW_FROM_V_DIF(Am, 1, data->orig_p[2], data->orig_p[0]);                                                      \
                            AE_MATRIX_F_SET_ROW_FROM_V(Am, 2, bn);                                                                                        \
                                                                                                                                                          \
                            AE_MATRIX_F_INVERSE(AIm, Am, 3, 3);                                                                                           \
                                                                                                                                                          \
                            for (size_t i = 0; i < 3; i++)                                                                                                \
                            {                                                                                                                             \
                                i_v.raw[i] = (AE_MATRIX_F_GET(AIm, i, 0) * (data->uvs[1].u - data->uvs[0].u) +                                            \
                                              AE_MATRIX_F_GET(AIm, i, 1) * (data->uvs[2].u - data->uvs[0].u));                                            \
                                j_v.raw[i] = (AE_MATRIX_F_GET(AIm, i, 0) * (data->uvs[1].v - data->uvs[0].v) +                                            \
                                              AE_MATRIX_F_GET(AIm, i, 1) * (data->uvs[2].v - data->uvs[0].v));                                            \
                            }                                                                                                                             \
                            AE_VEC3_NORMALIZE(i_v, i_v, 1);                                                                                               \
                            AE_VEC3_NORMALIZE(j_v, j_v, 1);                                                                                               \
                                                                                                                                                          \
                            AE_MATRIX_F_SET_COL_FROM_V(Bm, 0, i_v);                                                                                       \
                            AE_MATRIX_F_SET_COL_FROM_V(Bm, 1, j_v);                                                                                       \
                            AE_MATRIX_F_SET_COL_FROM_V(Bm, 2, bn);                                                                                        \
                                                                                                                                                          \
                            t_normal = normal_map_ae_model(model, &uv);                                                                                   \
                            AE_MATRIX_3x3_MULT_V(normal, Bm, t_normal);                                                                                   \
                            AE_VEC3_NORMALIZE(normal, normal, 1);                                                                                         \
                            diff = AE_VEC3_DOT(normal, l);                                                                                                \
                                                                                                                                                          \
                            r = normal;                                                                                                                   \
                            AE_VEC3_F_MULT(r, r, diff * 2.0);                                                                                             \
                            AE_VEC3_DIF(r, r, l);                                                                                                         \
                            AE_VEC3_NORMALIZE(r, r, 1);                                                                                                   \
                            r.z = max_ae(r.z, 0.0);                                                                                                       \
                            spec = pow(r.z, specular_map_ae_model(model, &uv));                                                                           \
                                                                                                                                                          \
                            diff = max_ae(diff, 0.0);                                                                                                     \
                                                                                                                                                          \
                            ae_tga_c new_color = texture_ae_model(model, &uv);                                                                            \
                                                                                                                                                          \
                            intensity = 1.0 * diff + 0.3 * spec;                                                                                          \
                            new_color.raw[0] = 5 + min_ae(new_color.raw[0] * intensity, 255);                                                             \
                            new_color.raw[1] = 5 + min_ae(new_color.raw[1] * intensity, 255);                                                             \
                            new_color.raw[2] = 5 + min_ae(new_color.raw[2] * intensity, 255);                                                             \
                                                                                                                                                          \
                            set_ae_tga(image, (int32_t)P.x, (int32_t)P.y, &new_color);                                                                    \
                                                                                                                                                          \
                            zbuffer[zbuffer_idx] = P.z;                                                                                                   \
                        }                                                                                                                                 \
                    }                                                                                                                                     \
                }                                                                                                                                         \
            }                                                                                                                                             \
    }

void triangle_ae_render(ae_tga_i *image, ae_model const *model, ae_render_data const *data, double *zbuffer)
{
    AE_VEC2_I_CREATE_FROM(point0, data->p[0]);
    AE_VEC2_I_CREATE_FROM(point1, data->p[1]);
    AE_VEC2_I_CREATE_FROM(point2, data->p[2]);

    if (point0.y == point1.y && point0.y == point2.y)
        return;

    if (point0.y > point1.y)
    {
        swap_ae(ae_vec2_i, point0, point1);
    }
    if (point0.y > point2.y)
    {
        swap_ae(ae_vec2_i, point0, point2);
    }
    if (point1.y > point2.y)
    {
        swap_ae(ae_vec2_i, point1, point2);
    }

    ae_vec3_f P;
    int32_t total_height = (point2.y - point0.y) == 0 ? 1 : (point2.y - point0.y);
    int32_t segment_height = (point1.y - point0.y) == 0 ? 1 : (point1.y - point0.y);
    double alpha;
    double beta;
    double A;
    double B;
    ae_vec2_f uv;
    double diff;
    double spec;
    double intensity;
    ae_vec3_f bc_screen;
    int32_t zbuffer_idx;
    ae_vec3_f r;
    ae_vec3_f normal;
    ae_vec3_f t_normal;
    ae_vec3_f bn;
    AE_MATRIX_F_CREATE(Am, 3, 3);
    AE_MATRIX_F_CREATE(AIm, 3, 3);
    AE_MATRIX_F_CREATE(Bm, 3, 3);
    ae_vec3_f i_v;
    ae_vec3_f j_v;

    AE_TRIANGLE_RENDER_CYCLE(0, 1);
    segment_height = (point2.y - point1.y) == 0 ? 1 : (point2.y - point1.y);
    AE_TRIANGLE_RENDER_CYCLE(1, 2);

    return;
}
