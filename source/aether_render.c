#include "aether_render.h"
#include "aether_model.h"
#include "aether_utils.h"
#include <math.h>
#include <float.h>

ae_vec3_f barycentric_ae_render(ae_vec3_f A, ae_vec3_f B, ae_vec3_f C, ae_vec3_f P)
{
    ae_vec3_f s[2];
    for (size_t i = 0; i < 2; i++)
    {
        s[i].raw[0] = C.raw[i] - A.raw[i];
        s[i].raw[1] = B.raw[i] - A.raw[i];
        s[i].raw[2] = A.raw[i] - P.raw[i];
    }
    ae_vec3_f u;
    AE_VEC3_CROSS(u, s[0], s[1]);
    if (fabs(u.raw[2]) > 1e-2)
    {
        ae_vec3_f ret;
        ret.x = 1.f - (u.x + u.y) / u.z;
        ret.y = u.y / u.z;
        ret.z = u.x / u.z;
        return ret;
    }
    u.x = -1;
    u.y = -1;
    u.z = -1;
    return u;
}

// void triangle_bbox_ae_render(ae_tga_i *image, ae_tga_i *texture, ae_vec3_f *pts, ae_vec2_f *uvs, double intensity, double *zbuffer)
// {
//     ae_vec2_f bboxmin = {.x = DBL_MAX, .y = DBL_MAX};
//     ae_vec2_f bboxmax = {.x = -DBL_MAX, .y = -DBL_MAX};
//     ae_vec2_f clamp = {.x = image->width - 1, .y = image->height - 1};

//     for (size_t i = 0; i < 3; i++)
//     {
//         for (size_t j = 0; j < 2; j++)
//         {
//             bboxmin.raw[j] = fmax(0.f, fmin(bboxmin.raw[j], pts[i].raw[j]));
//             bboxmax.raw[j] = fmin(clamp.raw[j], fmax(bboxmax.raw[j], pts[i].raw[j]));
//         }
//     }

//     ae_vec3_f P;
//     for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
//     {
//         for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
//         {
//             ae_vec3_f bc_screen = barycentric_ae_render(pts[0], pts[1], pts[2], P);
//             if (bc_screen.raw[0] < 0 || bc_screen.raw[1] < 0 || bc_screen.raw[2] < 0)
//                 continue;
//             P.z = 0;

//             for (size_t i = 0; i < 3; i++)
//                 P.z += pts[i].raw[2] * bc_screen.raw[i];
//             if (zbuffer[(int)(P.x + P.y * image->width)] < P.z)
//             {
//                 zbuffer[(int)(P.x + P.y * image->width)] = P.z;
//                 ae_vec2_f uv;
//                 uv.u = bc_screen.raw[0] * uvs[0].u + bc_screen.raw[1] * uvs[1].u + bc_screen.raw[2] * uvs[2].u;
//                 uv.v = bc_screen.raw[0] * uvs[0].v + bc_screen.raw[1] * uvs[1].v + bc_screen.raw[2] * uvs[2].v;
//                 ae_tga_c new_color = get_ae_tga(texture, uv.u, uv.v);
//                 new_color.r *= intensity;
//                 new_color.g *= intensity;
//                 new_color.b *= intensity;
//                 set_ae_tga(image, P.x, P.y, &new_color);
//             }
//         }
//     }
// }

void triangle_ae_render(ae_tga_i *image, ae_tga_i *texture, ae_vec3_f *p, ae_vec2_f *uvs, double intensity, double *zbuffer)
{
    if (p[0].y == p[1].y && p[0].y == p[2].y)
        return;

    if (p[0].y > p[1].y)
    {
        swap_ae(ae_vec3_f, p[0], p[1]);
        swap_ae(ae_vec2_f, uvs[0], uvs[1]);
    }
    if (p[0].y > p[2].y)
    {
        swap_ae(ae_vec3_f, p[0], p[2]);
        swap_ae(ae_vec2_f, uvs[0], uvs[2]);
    }
    if (p[1].y > p[2].y)
    {
        swap_ae(ae_vec3_f, p[1], p[2]);
        swap_ae(ae_vec2_f, uvs[1], uvs[2]);
    }

    double alpha;
    double beta;

    ae_vec3_f A1;
    ae_vec3_f A2;

    ae_vec3_f B1;
    ae_vec3_f B2;

    size_t i;
    size_t j;

    int32_t p0_y = (int32_t)p[0].y;
    int32_t p1_y = (int32_t)p[1].y;
    int32_t p2_y = (int32_t)p[2].y;

    int32_t total_height = p2_y - p0_y;
    total_height = total_height == 0 ? 1 : total_height;
    int32_t segment_height = p1_y - p0_y;
    segment_height = segment_height == 0 ? 1 : segment_height;

    ae_vec3_f P;

    AE_VEC3_DIF(A1, p[2], p[0]);
    AE_VEC3_DIF(B1, p[1], p[0]);

    if (p1_y >= 0)
        for (i = (p0_y >= 0 ? p0_y : 0); i <= ((p1_y < image->height) ? p1_y : image->height - 1); i++)
        {
            alpha = (double)(i - p0_y) / total_height;
            beta = (double)(i - p0_y) / segment_height;
            AE_VEC3_F_MULT(A2, A1, alpha);
            AE_VEC3_SUM(A2, A2, p[0]);

            AE_VEC3_F_MULT(B2, B1, beta);
            AE_VEC3_SUM(B2, B2, p[0]);

            if (A2.x > B2.x)
                swap_ae(ae_vec3_f, A2, B2);
            if (B2.x >= 0)
                for (j = (int32_t)A2.x >= 0 ? (int32_t)A2.x : 0; j <= (int32_t)((B2.x < image->width) ? B2.x : image->width - 1); j++)
                {
                    P.x = j;
                    P.y = i;
                    P.z = 0;
                    ae_vec3_f bc_screen = barycentric_ae_render(p[0], p[1], p[2], P);
                    for (size_t k = 0; k < 3; k++)
                        P.z += p[k].raw[2] * bc_screen.raw[k];
                    if (((int32_t)P.x + (int32_t)P.y * image->width) >= image->width * image->height)
                        continue;
                    if (zbuffer[(int32_t)P.x + (int32_t)P.y * image->width] < P.z)
                    {
                        ae_vec2_f uv;
                        uv.u = bc_screen.raw[0] * uvs[0].u + bc_screen.raw[1] * uvs[1].u + bc_screen.raw[2] * uvs[2].u;
                        uv.v = bc_screen.raw[0] * uvs[0].v + bc_screen.raw[1] * uvs[1].v + bc_screen.raw[2] * uvs[2].v;
                        ae_tga_c new_color = get_ae_tga(texture, uv.u, uv.v);

                        new_color.r *= intensity;
                        new_color.g *= intensity;
                        new_color.b *= intensity;

                        set_ae_tga(image, j, i, &new_color);

                        zbuffer[j + i * image->width] = P.z;
                    }
                }
        }

    segment_height = p2_y - p1_y;
    segment_height = segment_height == 0 ? 1 : segment_height;

    AE_VEC3_DIF(B1, p[2], p[1]);
    if (p2_y >= 0)
        for (i = (p1_y >= 0 ? p1_y : 0); i <= ((p2_y < image->height) ? p2_y : image->height - 1); i++)
        {
            alpha = (double)(i - p0_y) / total_height;
            beta = (double)(i - p1_y) / segment_height;

            AE_VEC3_F_MULT(A2, A1, alpha);
            AE_VEC3_SUM(A2, A2, p[0]);

            AE_VEC3_F_MULT(B2, B1, beta);
            AE_VEC3_SUM(B2, B2, p[1]);

            if (A2.x > B2.x)
                swap_ae(ae_vec3_f, A2, B2);

            if (B2.x >= 0)
                for (j = (int32_t)A2.x >= 0 ? (int32_t)A2.x : 0; j <= (int32_t)((B2.x < image->width) ? B2.x : image->width - 1); j++)
                {
                    P.x = j;
                    P.y = i;
                    P.z = 0;
                    ae_vec3_f bc_screen = barycentric_ae_render(p[0], p[1], p[2], P);
                    for (size_t k = 0; k < 3; k++)
                        P.z += p[k].raw[2] * bc_screen.raw[k];
                    if (((int32_t)P.x + (int32_t)P.y * image->width) >= image->width * image->height)
                        continue;
                    if (zbuffer[(int32_t)P.x + (int32_t)P.y * image->width] < P.z)
                    {
                        ae_vec2_f uv;
                        uv.u = bc_screen.raw[0] * uvs[0].u + bc_screen.raw[1] * uvs[1].u + bc_screen.raw[2] * uvs[2].u;
                        uv.v = bc_screen.raw[0] * uvs[0].v + bc_screen.raw[1] * uvs[1].v + bc_screen.raw[2] * uvs[2].v;
                        ae_tga_c new_color = get_ae_tga(texture, uv.u, uv.v);

                        new_color.r *= intensity;
                        new_color.g *= intensity;
                        new_color.b *= intensity;

                        set_ae_tga(image, j, i, &new_color);

                        zbuffer[j + i * image->width] = P.z;
                    }
                }
        }

    return;
}

void line_ae_render(ae_vec3_f p1, ae_vec3_f p2, ae_tga_i *image, ae_tga_c *color)
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

static AE_TGA_C_RGBA(red, 255, 0, 0, 255);
static AE_TGA_C_RGBA(green, 0, 255, 0, 255);

#define AE_DEFORM(vp, v, out)          \
    {                                   \
        AE_V2M_RENDER(v_1, v);          \
        AE_MATRIX_F_MULT(v_2, vp, v_1); \
        AE_M2V_RENDER(v_3, v_2);        \
        out = v_3;                      \
    }

ae_vec3_f camera = {.x = 0.0,
                    .y = 0.0,
                    .z = 3.0};

void render_model(ae_model *model, ae_tga_i *image)
{
    double *zbuffer = (double *)(calloc(image->width * image->height, sizeof(double)));
    for (size_t i = 0; i < image->width * image->height; i++)
    {
        zbuffer[i] = -DBL_MAX;
    }

    AE_VIEWPORT_RENDER(VP, 255, image->width / 4, image->width / 4, image->width / 2, image->height / 2);
    AE_MATRIX_F_IDENTITY(Proj, 4);
    AE_MATRIX_F_GET(Proj, 3, 2) = -1.0 / camera.z;

    AE_ZOOM_RENDER(T, 1.25);
    ae_vec3_f x = {.x = 1.0, .y = 0.0, .z = 0.0};
    ae_vec3_f y = {.x = 0.0, .y = 1.0, .z = 0.0};
    ae_vec3_f o = {.x = 0.0, .y = 0.0, .z = 0.0};

    AE_DEFORM(VP, o, o);
    AE_DEFORM(VP, x, x);
    AE_DEFORM(VP, y, y);

    line_ae_render(o, x, image, &red);
    line_ae_render(o, y, image, &green);

    ae_vec3_f light_dir = {.x = 0.0, .y = 0.0, .z = -1.0};
    for (int i = 0; i < n_faces_ae_model(model); i++)
    {
        ae_vector face;
        face_ae_model(model, i, &face);
        ae_face face_data;
        ae_vec2_f world_uv[3];
        ae_vec3_f screen_coords[3];
        ae_vec3_f world_coords[3];
        for (int j = 0; j < 3; j++)
        {
            get_ae_vector(&face, j, &face_data);
            vert_ae_model(model, face_data.v_i, &world_coords[j]);
            uv_ae_model(model, face_data.uv_i, &world_uv[j]);

            world_uv[j].u = (1.0 - world_uv[j].u) * model->texture.width;
            world_uv[j].v = (1.0 - world_uv[j].v) * model->texture.height;

            AE_V2M_RENDER(v_1, world_coords[j]);
            AE_MATRIX_F_MULT(v_2, VP, Proj);
            AE_MATRIX_F_MULT(v_3, v_2, T);
            AE_ROTATION_X_RENDER(rx,1.0, 0.0);
            AE_ROTATION_Y_RENDER(ry,1.0, 0.3);
            AE_ROTATION_Z_RENDER(rz,1.0, 0.0);
            AE_MATRIX_F_MULT(v_4, v_3, rx);
            AE_MATRIX_F_MULT(v_5, v_4, ry);
            AE_MATRIX_F_MULT(v_6, v_5, ry);
            AE_MATRIX_F_MULT(v_7, v_6, v_1);

            AE_M2V_RENDER(v_8, v_7);
            screen_coords[j] = v_8;
        }
        ae_vec3_f n;
        {
            ae_vec3_f n1, n2;
            AE_VEC3_DIF(n1, world_coords[1], world_coords[0]);
            AE_VEC3_DIF(n2, world_coords[2], world_coords[0]);
            AE_VEC3_CROSS(n, n2, n1);
        }
        AE_VEC3_NORMALIZE(n, n, 1.0);
        double intensity = AE_VEC3_DOT(n, light_dir);
        if (intensity > 0.0)
        {
            triangle_ae_render(image, &model->texture, screen_coords, world_uv, intensity, zbuffer);
        }
    }

    free(zbuffer);

    return;
}
