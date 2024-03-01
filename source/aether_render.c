// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_render.h"
#include "aether_model.h"
#include "aether_utils.h"
#include <math.h>
#include <float.h>

ae_vec3_f barycentric_ae_render(ae_vec3_f *A, ae_vec3_f *P)
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

#define AE_TRIANGLE_RENDER_CYCLE(n1, n2)                                                                                                                              \
    {                                                                                                                                                          \
        if (point##n2.y >= 0)                                                                                                                                  \
            for (size_t i = (point##n1.y >= 0 ? point##n1.y : 0); i <= ((point##n2.y < image->height) ? point##n2.y : image->height - 1); i++)                 \
            {                                                                                                                                                  \
                alpha = (double)(i - point0.y) / total_height;                                                                                                 \
                beta = (double)(i - point##n1.y) / segment_height;                                                                                             \
                                                                                                                                                               \
                P.y = i;                                                                                                                                       \
                                                                                                                                                               \
                A = point0.x + (point2.x - point0.x) * alpha;                                                                                                  \
                B = point##n1.x + (point##n2.x - point##n1.x) * beta;                                                                                          \
                                                                                                                                                               \
                if (A > B)                                                                                                                                     \
                {                                                                                                                                              \
                    swap_ae(double, A, B);                                                                                                                     \
                }                                                                                                                                              \
                if (B >= 0)                                                                                                                                    \
                {                                                                                                                                              \
                    for (size_t j = (int32_t)A >= 0 ? (int32_t)A : 0; j <= (((int32_t)B < image->width) ? (int32_t)B : image->width - 1); j++)                 \
                    {                                                                                                                                          \
                        P.x = j;                                                                                                                               \
                        bc_screen = barycentric_ae_render(data->p, &P);                                                                                        \
                        zbuffer_idx = (int32_t)P.x + (int32_t)P.y * image->width;                                                                              \
                        if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)                                                                             \
                            continue;                                                                                                                          \
                        if (zbuffer_idx < image->width * image->height && zbuffer[zbuffer_idx] < P.z)                                                          \
                        {                                                                                                                                      \
                            uv.u = bc_screen.raw[0] * data->uvs[0].u + bc_screen.raw[1] * data->uvs[1].u + bc_screen.raw[2] * data->uvs[2].u;                  \
                            uv.v = bc_screen.raw[0] * data->uvs[0].v + bc_screen.raw[1] * data->uvs[1].v + bc_screen.raw[2] * data->uvs[2].v;                  \
                                                                                                                                                               \
                            intensity = bc_screen.raw[0] * data->intensity[0] + bc_screen.raw[1] * data->intensity[1] + bc_screen.raw[2] * data->intensity[2]; \
                            intensity = intensity < 0.0 ? 0.0 : intensity;                                                                                     \
                                                                                                                                                               \
                            AE_TGA_C_RGBA(new_color, 255, 255, 255, 255);                                                                                      \
                            if (model->texture.data != NULL)                                                                                                   \
                                new_color = get_ae_tga(&model->texture, uv.u, uv.v);                                                                           \
                                                                                                                                                               \
                            AE_TGA_SET_INTENSITY(new_color, intensity);                                                                                        \
                                                                                                                                                               \
                            set_ae_tga(image, (int32_t)P.x, (int32_t)P.y, &new_color);                                                                         \
                                                                                                                                                               \
                            zbuffer[zbuffer_idx] = P.z;                                                                                                        \
                        }                                                                                                                                      \
                    }                                                                                                                                          \
                }                                                                                                                                              \
            }                                                                                                                                                  \
    }

void triangle_ae_render(ae_tga_i *image, ae_model *model, ae_render_data *data, double *zbuffer)
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
    double intensity;
    ae_vec3_f bc_screen;
    int32_t zbuffer_idx;

    AE_TRIANGLE_RENDER_CYCLE(0, 1);
    segment_height = (point2.y - point1.y) == 0 ? 1 : (point2.y - point1.y);
    AE_TRIANGLE_RENDER_CYCLE(1, 2);

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

void render_model(ae_model *model, ae_tga_i *image)
{
    double *zbuffer = (double *)(calloc(image->width * image->height, sizeof(double)));
    for (size_t i = 0; i < image->width * image->height; i++)
    {
        zbuffer[i] = -DBL_MAX;
    }

    ae_vec3_f light_dir = {.x = 1.0, .y = -0.0, .z = 1.0};
    AE_VEC3_NORMALIZE(light_dir, light_dir, 1);

    ae_vec3_f eye = {.x = 0.0, .y = 0.0, .z = 5.0};
    ae_vec3_f center = {.x = 0.0, .y = 0.0, .z = -5.0};
    ae_vec3_f up = {.x = 0.0, .y = 1.0, .z = 0.0};

    AE_LOOK_AT(ModelView, eye, center, up);
    AE_VIEWPORT_RENDER(VP, 255, image->width / 3, image->height / 4, image->width / 3, image->height / 2);
    AE_MATRIX_F_IDENTITY(Proj, 4);
    {
        ae_vec3_f dif;
        AE_VEC3_DIF(dif, eye, center);
        AE_MATRIX_F_GET(Proj, 3, 2) = -1.0 / AE_VEC3_NORM(dif);
    }

    ae_vec3_f x = {.x = 10.0, .y = 0.0, .z = 0.0};
    ae_vec3_f y = {.x = 0.0, .y = 10.0, .z = 0.0};
    ae_vec3_f o = {.x = 0.0, .y = 0.0, .z = 0.0};

    AE_DEFORM(VP, o);
    AE_DEFORM(VP, x);
    AE_DEFORM(VP, y);

    line_ae_render(o, x, image, &red);
    line_ae_render(o, y, image, &green);

    double X_deg = 0.0;
    double Y_deg = 0.0;
    double Z_deg = 0.0;

    AE_ROTATION_X_RENDER(rx, cos(X_deg * PI / 180), sin(X_deg * PI / 180));
    AE_ROTATION_Y_RENDER(ry, cos(Y_deg * PI / 180), sin(Y_deg * PI / 180));
    AE_ROTATION_Z_RENDER(rz, cos(Z_deg * PI / 180), sin(Z_deg * PI / 180));

    AE_MATRIX_F_MULT(m_2, VP, Proj, 4, 4);
    AE_MATRIX_F_MULT(m_3, m_2, ModelView, 4, 4);
    AE_MATRIX_F_MULT(m_4, m_3, rx, 4, 4);
    AE_MATRIX_F_MULT(m_5, m_4, ry, 4, 4);
    AE_MATRIX_F_MULT(Z, m_5, ry, 4, 4);

    ae_face face[3];
    ae_render_data render_data;
    ae_vec3_f normal;

    for (size_t i = 0; i < n_faces_ae_model(model); i++)
    {
        face_ae_model(model, i, face);
        for (size_t j = 0; j < 3; j++)
        {
            vert_ae_model(model, face[j].v_i, &render_data.p[j]);

            AE_V2M_RENDER(wc_m, render_data.p[j]);
            AE_MATRIX_F_MULT(Z_m, Z, wc_m, 4, 1);
            AE_M2VI_RENDER(render_data.p[j], Z_m);

            uv_ae_model(model, face[j].uv_i, &render_data.uvs[j]);
            render_data.uvs[j].u = render_data.uvs[j].u * model->texture.width;
            render_data.uvs[j].v = render_data.uvs[j].v * model->texture.height;

            normal_ae_model(model, face[j].norm_i, &normal);
            AE_VEC3_NORMALIZE(normal, normal, 1);
            render_data.intensity[j] = AE_VEC3_DOT(normal, light_dir);
        }

        triangle_ae_render(image, model, &render_data, zbuffer);
    }

    free(zbuffer);

    return;
}
