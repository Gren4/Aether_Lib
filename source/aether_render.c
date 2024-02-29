// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_render.h"
#include "aether_model.h"
#include "aether_utils.h"
#include <math.h>
#include <float.h>

void triangle_ae_render(ae_tga_i *image, ae_tga_i *texture, ae_render_data* data, double *zbuffer)
{
    if (data->p[0].y == data->p[1].y && data->p[0].y == data->p[2].y)
        return;

    if (data->p[0].y > data->p[1].y)
    {
        swap_ae(ae_vec3_f, data->p[0], data->p[1]);
        swap_ae(ae_vec2_f, data->uvs[0], data->uvs[1]);
        swap_ae(double, data->intensity[0], data->intensity[1]);
    }
    if (data->p[0].y > data->p[2].y)
    {
        swap_ae(ae_vec3_f, data->p[0], data->p[2]);
        swap_ae(ae_vec2_f, data->uvs[0], data->uvs[2]);
        swap_ae(double, data->intensity[0], data->intensity[2]);
    }
    if (data->p[1].y > data->p[2].y)
    {
        swap_ae(ae_vec3_f, data->p[1], data->p[2]);
        swap_ae(ae_vec2_f, data->uvs[1], data->uvs[2]);
        swap_ae(double, data->intensity[1], data->intensity[2]);
    }

    double alpha;
    double beta;

    ae_vec3_f A;
    ae_vec3_f B;

    ae_vec2_f uvA;
    ae_vec2_f uvB;

    double intensityA;
    double intensityB;

    ae_vec3_f P;

    ae_vec2_f uvP;

    double intensityP;

    double phi;

    AE_VEC3_I_CREATE_FROM(p_i0, data->p[0]);
    AE_VEC3_I_CREATE_FROM(p_i1, data->p[1]);
    AE_VEC3_I_CREATE_FROM(p_i2, data->p[2]);

    int32_t total_height = p_i2.y - p_i0.y;
    total_height = (total_height == 0) ? 1 : total_height;

    int32_t segment_height = p_i1.y - p_i0.y;
    segment_height = (segment_height == 0) ? 1 : segment_height;

    int32_t zbuffer_idx;

    if (p_i1.y >= 0)
        for (size_t i = (p_i0.y >= 0 ? p_i0.y : 0); i <= ((p_i1.y < image->height) ? p_i1.y : image->height - 1); i++)
        {
            alpha = (double)(i - p_i0.y) / total_height;
            beta = (double)(i - p_i0.y) / segment_height;

            AE_VEC3_V_PLUS_DIF_MULT(A, data->p[0], data->p[2], data->p[0], alpha);
            AE_VEC3_V_PLUS_DIF_MULT(B, data->p[0], data->p[1], data->p[0], beta);
            AE_VEC2_V_PLUS_DIF_MULT(uvA, data->uvs[0], data->uvs[2], data->uvs[0], alpha);
            AE_VEC2_V_PLUS_DIF_MULT(uvB, data->uvs[0], data->uvs[1], data->uvs[0], beta);
            intensityA = data->intensity[0] + (data->intensity[2] - data->intensity[0]) * alpha;
            intensityB = data->intensity[0] + (data->intensity[1] - data->intensity[0]) * beta;

            if (A.x > B.x)
            {
                swap_ae(ae_vec3_f, A, B);
                swap_ae(ae_vec2_f, uvA, uvB);
                swap_ae(double, intensityA, intensityB);
            }
            if (B.x >= 0)
            {
                AE_VEC3_I_CREATE_FROM(A_i, A);
                AE_VEC3_I_CREATE_FROM(B_i, B);

                for (size_t j = A_i.x >= 0 ? A_i.x : 0; j <= ((B_i.x < image->width) ? B_i.x : image->width - 1); j++)
                {
                    phi = (B_i.x == A_i.x) ? 1.0 : (double)(j - A_i.x) / (double)(B_i.x - A_i.x);

                    AE_VEC3_V_PLUS_DIF_MULT(P, A, B, A, phi);

                    zbuffer_idx = (int32_t)P.x + (int32_t)P.y * image->width;

                    if (zbuffer_idx >= image->width * image->height)
                        continue;
                    if (zbuffer[zbuffer_idx] < P.z)
                    {
                        AE_VEC2_V_PLUS_DIF_MULT(uvP, uvA, uvB, uvA, phi);
                        intensityP = intensityA + (intensityB - intensityA) * phi;
                        intensityP = intensityP < 0.0 ? 0.0 : intensityP;

                        ae_tga_c new_color = get_ae_tga(texture, uvP.u, uvP.v);

                        new_color.r *= intensityP;
                        new_color.g *= intensityP;
                        new_color.b *= intensityP;

                        set_ae_tga(image, (int32_t)P.x, (int32_t)P.y, &new_color);

                        zbuffer[zbuffer_idx] = P.z;
                    }
                }
            }
        }

    segment_height = p_i2.y - p_i1.y;
    segment_height = segment_height == 0 ? 1 : segment_height;

    if (p_i2.y >= 0)
        for (size_t i = (p_i1.y >= 0 ? p_i1.y : 0); i <= ((p_i2.y < image->height) ? p_i2.y : image->height - 1); i++)
        {
            alpha = (double)(i - p_i0.y) / total_height;
            beta = (double)(i - p_i1.y) / segment_height;

            AE_VEC3_V_PLUS_DIF_MULT(A, data->p[0], data->p[2], data->p[0], alpha);
            AE_VEC3_V_PLUS_DIF_MULT(B, data->p[1], data->p[2], data->p[1], beta);
            AE_VEC2_V_PLUS_DIF_MULT(uvA, data->uvs[0], data->uvs[2], data->uvs[0], alpha);
            AE_VEC2_V_PLUS_DIF_MULT(uvB, data->uvs[1], data->uvs[2], data->uvs[1], beta);
            intensityA = data->intensity[0] + (data->intensity[2] - data->intensity[0]) * alpha;
            intensityB = data->intensity[1] + (data->intensity[2] - data->intensity[1]) * beta;

            if (A.x > B.x)
            {
                swap_ae(ae_vec3_f, A, B);
                swap_ae(ae_vec2_f, uvA, uvB);
                swap_ae(double, intensityA, intensityB);
            }
            if (B.x >= 0)
            {
                AE_VEC3_I_CREATE_FROM(A_i, A);
                AE_VEC3_I_CREATE_FROM(B_i, B);

                for (size_t j = A_i.x >= 0 ? A_i.x : 0; j <= ((B_i.x < image->width) ? B_i.x : image->width - 1); j++)
                {
                    phi = (B_i.x == A_i.x) ? 1.0 : (double)(j - A_i.x) / (double)(B_i.x - A_i.x);

                    AE_VEC3_V_PLUS_DIF_MULT(P, A, B, A, phi);

                    zbuffer_idx = (int32_t)P.x + (int32_t)P.y * image->width;

                    if (zbuffer_idx >= image->width * image->height)
                        continue;
                    if (zbuffer[zbuffer_idx] < P.z)
                    {
                        AE_VEC2_V_PLUS_DIF_MULT(uvP, uvA, uvB, uvA, phi);
                        intensityP = intensityA + (intensityB - intensityA) * phi;
                        intensityP = intensityP < 0.0 ? 0.0 : intensityP;

                        ae_tga_c new_color = get_ae_tga(texture, uvP.u, uvP.v);

                        new_color.r *= intensityP;
                        new_color.g *= intensityP;
                        new_color.b *= intensityP;

                        set_ae_tga(image, (int32_t)P.x, (int32_t)P.y, &new_color);

                        zbuffer[zbuffer_idx] = P.z;
                    }
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

void render_model(ae_model *model, ae_tga_i *image)
{
    double *zbuffer = (double *)(calloc(image->width * image->height, sizeof(double)));
    for (size_t i = 0; i < image->width * image->height; i++)
    {
        zbuffer[i] = -DBL_MAX;
    }

    ae_vec3_f light_dir = {.x = 1.0, .y = -0.0, .z = 1.0};
    AE_VEC3_NORMALIZE(light_dir, light_dir, 1);

    ae_vec3_f eye = {.x = -3.0, .y = 0.0, .z = 3.0};
    ae_vec3_f center = {.x = 0.0, .y = 0.0, .z = 0.0};
    ae_vec3_f up = {.x = 0.0, .y = 1.0, .z = 0.0};

    AE_LOOK_AT(ModelView, eye, center, up);
    AE_VIEWPORT_RENDER(VP, 255, image->width / 4, image->height / 4, image->width / 2, image->height / 1.5);
    AE_MATRIX_F_IDENTITY(Proj, 4);
    {
        ae_vec3_f dif;
        AE_VEC3_DIF(dif, eye, center);
        AE_MATRIX_F_GET(Proj, 3, 2) = -1.0 / AE_VEC3_NORM(dif);
    }

    ae_vec3_f x = {.x = 1.0, .y = 0.0, .z = 0.0};
    ae_vec3_f y = {.x = 0.0, .y = 1.0, .z = 0.0};
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

    ae_render_data render_data;
    ae_face face[3];
    ae_vec3_f normals;

    for (size_t i = 0; i < n_faces_ae_model(model); i++)
    {
        face_ae_model(model, i, face);
        for (size_t j = 0; j < 3; j++)
        {
            vert_ae_model(model, face[j].v_i, &render_data.p[j]);

            AE_V2M_RENDER(wc_m, render_data.p[j]);
            AE_MATRIX_F_MULT(Z_m, Z, wc_m, 4, 1);
            AE_M2V_RENDER(render_data.p[j], Z_m);

            uv_ae_model(model, face[j].uv_i, &render_data.uvs[j]);
            render_data.uvs[j].u = render_data.uvs[j].u * model->texture.width;
            render_data.uvs[j].v = render_data.uvs[j].v * model->texture.height;

            normal_ae_model(model, face[j].norm_i, &normals);
            AE_VEC3_NORMALIZE(normals, normals, 1);
            render_data.intensity[j] = AE_VEC3_DOT(normals, light_dir);
        }

        triangle_ae_render(image, &model->texture, &render_data, zbuffer);
    }

    free(zbuffer);

    return;
}
