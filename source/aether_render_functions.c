// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_render.h"
#include "aether_render_gv.h"
#include "aether_utils.h"

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

void triangle_ae_render(ae_tga_i *image, ae_model const *model, ae_shader const *shader, double *zbuffer)
{
    AE_VEC2_I_CREATE_FROM(point0, shader->p[0]);
    AE_VEC2_I_CREATE_FROM(point1, shader->p[1]);
    AE_VEC2_I_CREATE_FROM(point2, shader->p[2]);

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

    if (point1.y >= 0)
        for (size_t i = (point0.y >= 0 ? point0.y : 0); i <= ((point1.y < height) ? point1.y : height - 1); i++)
        {
            double alpha = (double)(i - point0.y) / total_height;
            double beta = (double)(i - point0.y) / segment_height;
            P.y = i;
            double A = point0.x + (point2.x - point0.x) * alpha;
            double B = point0.x + (point1.x - point0.x) * beta;
            if (A > B)
            {
                swap_ae(double, A, B);
            }
            if (B >= 0)
            {
                for (size_t j = (int32_t)A >= 0 ? (int32_t)A : 0; j <= (((int32_t)B < width) ? (int32_t)B : width - 1); j++)
                {
                    P.x = j;
                    ae_vec3_f bc_screen = barycentric_ae_render(shader->p, &P);
                    if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                        continue;
                    int32_t zbuffer_idx = (int32_t)P.x + (int32_t)P.y * width;
                    ae_vec3_f bc_clip = {.x = bc_screen.x / shader->coeff_p[0],
                                         .y = bc_screen.y / shader->coeff_p[1],
                                         .z = bc_screen.z / shader->coeff_p[2]};
                    double clip_coeff = 1.0 / (bc_clip.x + bc_clip.y + bc_clip.z);
                    AE_VEC3_F_MULT(bc_clip, bc_clip, clip_coeff);
                    if (zbuffer_idx < width * height && zbuffer[zbuffer_idx] < P.z)
                    {
                        ae_tga_c new_color;
                        shader->fragment(model, &bc_clip, &new_color);
                        set_ae_tga(image, (int32_t)P.x, (int32_t)P.y, &new_color);
                        zbuffer[zbuffer_idx] = P.z;
                    }
                }
            }
        }

    segment_height = (point2.y - point1.y) == 0 ? 1 : (point2.y - point1.y);

    if (point2.y >= 0)
        for (size_t i = (point1.y >= 0 ? point1.y : 0); i <= ((point2.y < height) ? point2.y : height - 1); i++)
        {
            double alpha = (double)(i - point0.y) / total_height;
            double beta = (double)(i - point1.y) / segment_height;
            P.y = i;
            double A = point0.x + (point2.x - point0.x) * alpha;
            double B = point1.x + (point2.x - point1.x) * beta;
            if (A > B)
            {
                swap_ae(double, A, B);
            }
            if (B >= 0)
            {
                for (size_t j = (int32_t)A >= 0 ? (int32_t)A : 0; j <= (((int32_t)B < width) ? (int32_t)B : width - 1); j++)
                {
                    P.x = j;
                    ae_vec3_f bc_screen = barycentric_ae_render(shader->p, &P);
                    if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                        continue;
                    int32_t zbuffer_idx = (int32_t)P.x + (int32_t)P.y * width;
                    ae_vec3_f bc_clip = {.x = bc_screen.x / shader->coeff_p[0],
                                         .y = bc_screen.y / shader->coeff_p[1],
                                         .z = bc_screen.z / shader->coeff_p[2]};
                    double clip_coeff = 1.0 / (bc_clip.x + bc_clip.y + bc_clip.z);
                    AE_VEC3_F_MULT(bc_clip, bc_clip, clip_coeff);
                    if (zbuffer_idx < width * height && zbuffer[zbuffer_idx] < P.z)
                    {
                        ae_tga_c new_color;
                        shader->fragment(model, &bc_clip, &new_color);
                        set_ae_tga(image, (int32_t)P.x, (int32_t)P.y, &new_color);
                        zbuffer[zbuffer_idx] = P.z;
                    }
                }
            }
        }

    return;
}

void shadow_buffer_ae_render(ae_tga_i *image, ae_model const *model, ae_shader const *shader, double *zbuffer)
{
    AE_VEC2_I_CREATE_FROM(point0, shader->p[0]);
    AE_VEC2_I_CREATE_FROM(point1, shader->p[1]);
    AE_VEC2_I_CREATE_FROM(point2, shader->p[2]);

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
    ae_vec3_f bc_screen;
    int32_t zbuffer_idx;

    if (point1.y >= 0)
        for (size_t i = (point0.y >= 0 ? point0.y : 0); i <= ((point1.y < height) ? point1.y : height - 1); i++)
        {
            alpha = (double)(i - point0.y) / total_height;
            beta = (double)(i - point0.y) / segment_height;
            P.y = i;
            A = point0.x + (point2.x - point0.x) * alpha;
            B = point0.x + (point1.x - point0.x) * beta;
            if (A > B)
            {
                swap_ae(double, A, B);
            }
            if (B >= 0)
            {
                for (size_t j = (int32_t)A >= 0 ? (int32_t)A : 0; j <= (((int32_t)B < width) ? (int32_t)B : width - 1); j++)
                {
                    P.x = j;
                    bc_screen = barycentric_ae_render(shader->p, &P);
                    if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                        continue;
                    zbuffer_idx = (int32_t)P.x + (int32_t)P.y * width;
                    ae_vec3_f bc_clip = {.x = bc_screen.x / shader->coeff_p[0],
                                         .y = bc_screen.y / shader->coeff_p[1],
                                         .z = bc_screen.z / shader->coeff_p[2]};
                    double clip_coeff = 1.0 / (bc_clip.x + bc_clip.y + bc_clip.z);
                    AE_VEC3_F_MULT(bc_clip, bc_clip, clip_coeff);
                    if (zbuffer_idx < width * height && zbuffer[zbuffer_idx] < P.z)
                    {
                        zbuffer[zbuffer_idx] = P.z;
                    }
                }
            }
        }

    segment_height = (point2.y - point1.y) == 0 ? 1 : (point2.y - point1.y);

    if (point2.y >= 0)
        for (size_t i = (point1.y >= 0 ? point1.y : 0); i <= ((point2.y < height) ? point2.y : height - 1); i++)
        {
            alpha = (double)(i - point0.y) / total_height;
            beta = (double)(i - point1.y) / segment_height;
            P.y = i;
            A = point0.x + (point2.x - point0.x) * alpha;
            B = point1.x + (point2.x - point1.x) * beta;
            if (A > B)
            {
                swap_ae(double, A, B);
            }
            if (B >= 0)
            {
                for (size_t j = (int32_t)A >= 0 ? (int32_t)A : 0; j <= (((int32_t)B < width) ? (int32_t)B : width - 1); j++)
                {
                    P.x = j;
                    bc_screen = barycentric_ae_render(shader->p, &P);
                    if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                        continue;
                    zbuffer_idx = (int32_t)P.x + (int32_t)P.y * width;
                    ae_vec3_f bc_clip = {.x = bc_screen.x / shader->coeff_p[0],
                                         .y = bc_screen.y / shader->coeff_p[1],
                                         .z = bc_screen.z / shader->coeff_p[2]};
                    double clip_coeff = 1.0 / (bc_clip.x + bc_clip.y + bc_clip.z);
                    AE_VEC3_F_MULT(bc_clip, bc_clip, clip_coeff);
                    if (zbuffer_idx < width * height && zbuffer[zbuffer_idx] < P.z)
                    {
                        zbuffer[zbuffer_idx] = P.z;
                    }
                }
            }
        }

    return;
}
