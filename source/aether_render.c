// based on https://github.com/ssloy/tinyrenderer/wiki
#include "aether_render.h"
#include "aether_model.h"
#include "aether_utils.h"
#include <math.h>
#include <float.h>

void triangle_ae_render(ae_tga_i *image, ae_tga_i *texture, ae_vec3_f *p, ae_vec2_f *uvs, double *intensity, double *zbuffer)
{
    if (p[0].y == p[1].y && p[0].y == p[2].y)
        return;

    if (p[0].y > p[1].y)
    {
        swap_ae(ae_vec3_f, p[0], p[1]);
        swap_ae(ae_vec2_f, uvs[0], uvs[1]);
        swap_ae(double, intensity[0], intensity[1]);
    }
    if (p[0].y > p[2].y)
    {
        swap_ae(ae_vec3_f, p[0], p[2]);
        swap_ae(ae_vec2_f, uvs[0], uvs[2]);
        swap_ae(double, intensity[0], intensity[2]);
    }
    if (p[1].y > p[2].y)
    {
        swap_ae(ae_vec3_f, p[1], p[2]);
        swap_ae(ae_vec2_f, uvs[1], uvs[2]);
        swap_ae(double, intensity[1], intensity[2]);
    }

    double alpha;
    double beta;

    ae_vec3_f A1;
    ae_vec3_f A2;

    ae_vec3_f B1;
    ae_vec3_f B2;

    ae_vec2_f uvA1;
    ae_vec2_f uvA2;

    ae_vec2_f uvB1;
    ae_vec2_f uvB2;

    double intensityA1;
    double intensityA2;

    double intensityB1;
    double intensityB2;

    ae_vec3_f P1;
    ae_vec3_f P2;

    ae_vec2_f uvP1;
    ae_vec2_f uvP2;

    double intensityP1;
    double intensityP2;

    double phi;

    int32_t p0_y = (int32_t)p[0].y;
    int32_t p1_y = (int32_t)p[1].y;
    int32_t p2_y = (int32_t)p[2].y;

    int32_t total_height = p2_y - p0_y;
    total_height = (total_height == 0) ? 1 : total_height;

    int32_t segment_height = p1_y - p0_y;
    segment_height = (segment_height == 0) ? 1 : segment_height;

    AE_VEC3_DIF(A1, p[2], p[0]);
    AE_VEC3_DIF(B1, p[1], p[0]);

    AE_VEC2_DIF(uvA1, uvs[2], uvs[0]);
    AE_VEC2_DIF(uvB1, uvs[1], uvs[0]);

    intensityA1 = intensity[2] - intensity[0];
    intensityB1 = intensity[1] - intensity[0];

    int32_t zbuffer_idx;

    if (p1_y >= 0)
        for (size_t i = (p0_y >= 0 ? p0_y : 0); i <= ((p1_y < image->height) ? p1_y : image->height - 1); i++)
        {
            alpha = (double)(i - p0_y) / total_height;
            beta = (double)(i - p0_y) / segment_height;

            AE_VEC3_F_MULT(A2, A1, alpha);
            AE_VEC3_SUM(A2, A2, p[0]);

            AE_VEC3_F_MULT(B2, B1, beta);
            AE_VEC3_SUM(B2, B2, p[0]);

            AE_VEC2_F_MULT(uvA2, uvA1, alpha);
            AE_VEC2_SUM(uvA2, uvA2, uvs[0]);

            AE_VEC2_F_MULT(uvB2, uvB1, beta);
            AE_VEC2_SUM(uvB2, uvB2, uvs[0]);

            intensityA2 = intensity[0] + intensityA1 * alpha;
            intensityB2 = intensity[0] + intensityB1 * beta;

            if (A2.x > B2.x)
            {
                swap_ae(ae_vec3_f, A2, B2);
                swap_ae(ae_vec2_f, uvA2, uvB2);
                swap_ae(double, intensityA2, intensityB2);
            }
            if (B2.x >= 0)
            {
                AE_VEC3_DIF(P1, B2, A2);

                AE_VEC2_DIF(uvP1, uvB2, uvA2);

                intensityP1 = intensityB2 - intensityA2;

                for (size_t j = (int32_t)A2.x >= 0 ? (int32_t)A2.x : 0; j <= (int32_t)((B2.x < image->width) ? B2.x : image->width - 1); j++)
                {
                    phi = ((int32_t)B2.x == (int32_t)A2.x) ? 1.0 : (double)(j - (int32_t)A2.x) / (double)((int32_t)B2.x - (int32_t)A2.x);

                    AE_VEC3_F_MULT(P2, P1, phi);

                    AE_VEC3_SUM(P2, A2, P2);

                    zbuffer_idx = (int32_t)P2.x + (int32_t)P2.y * image->width;

                    if (zbuffer_idx >= image->width * image->height)
                        continue;
                    if (zbuffer[zbuffer_idx] < P2.z)
                    {
                        AE_VEC2_F_MULT(uvP2, uvP1, phi);

                        AE_VEC2_SUM(uvP2, uvA2, uvP2);

                        intensityP2 = intensityA2 + intensityP1 * phi;
                        intensityP2 = intensityP2 < 0.0 ? 0.0 : intensityP2;

                        ae_tga_c new_color = get_ae_tga(texture, uvP2.u, uvP2.v);

                        new_color.r *= intensityP2;
                        new_color.g *= intensityP2;
                        new_color.b *= intensityP2;

                        set_ae_tga(image, (int32_t)P2.x, (int32_t)P2.y, &new_color);

                        zbuffer[zbuffer_idx] = P2.z;
                    }
                }
            }
        }

    segment_height = p2_y - p1_y;
    segment_height = segment_height == 0 ? 1 : segment_height;

    AE_VEC3_DIF(B1, p[2], p[1]);

    AE_VEC2_DIF(uvB1, uvs[2], uvs[1]);

    intensityB1 = intensity[2] - intensity[1];

    if (p2_y >= 0)
        for (size_t i = (p1_y >= 0 ? p1_y : 0); i <= ((p2_y < image->height) ? p2_y : image->height - 1); i++)
        {
            alpha = (double)(i - p0_y) / total_height;
            beta = (double)(i - p1_y) / segment_height;

            AE_VEC3_F_MULT(A2, A1, alpha);
            AE_VEC3_SUM(A2, A2, p[0]);

            AE_VEC3_F_MULT(B2, B1, beta);
            AE_VEC3_SUM(B2, B2, p[1]);

            AE_VEC2_F_MULT(uvA2, uvA1, alpha);
            AE_VEC2_SUM(uvA2, uvA2, uvs[0]);

            AE_VEC2_F_MULT(uvB2, uvB1, beta);
            AE_VEC2_SUM(uvB2, uvB2, uvs[1]);

            intensityA2 = intensity[0] + intensityA1 * alpha;
            intensityB2 = intensity[1] + intensityB1 * beta;

            if (A2.x > B2.x)
            {
                swap_ae(ae_vec3_f, A2, B2);
                swap_ae(ae_vec2_f, uvA2, uvB2);
                swap_ae(double, intensityA2, intensityB2);
            }

            if (B2.x >= 0)
            {
                AE_VEC3_DIF(P1, B2, A2);

                AE_VEC2_DIF(uvP1, uvB2, uvA2);

                intensityP1 = intensityB2 - intensityA2;

                for (size_t j = (int32_t)A2.x >= 0 ? (int32_t)A2.x : 0; j <= (int32_t)((B2.x < image->width) ? B2.x : image->width - 1); j++)
                {
                    phi = ((int32_t)B2.x == (int32_t)A2.x) ? 1.0 : (double)(j - (int32_t)A2.x) / (double)((int32_t)B2.x - (int32_t)A2.x);

                    AE_VEC3_F_MULT(P2, P1, phi);

                    AE_VEC3_SUM(P2, A2, P2);

                    zbuffer_idx = (int32_t)P2.x + (int32_t)P2.y * image->width;

                    if (zbuffer_idx >= image->width * image->height)
                        continue;
                    if (zbuffer[zbuffer_idx] < P2.z)
                    {
                        AE_VEC2_F_MULT(uvP2, uvP1, phi);

                        AE_VEC2_SUM(uvP2, uvA2, uvP2);

                        intensityP2 = intensityA2 + intensityP1 * phi;
                        intensityP2 = intensityP2 < 0.0 ? 0.0 : intensityP2;

                        ae_tga_c new_color = get_ae_tga(texture, uvP2.u, uvP2.v);

                        new_color.r *= intensityP2;
                        new_color.g *= intensityP2;
                        new_color.b *= intensityP2;

                        set_ae_tga(image, (int32_t)P2.x, (int32_t)P2.y, &new_color);

                        zbuffer[zbuffer_idx] = P2.z;
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

    ae_face face[3];

    ae_vec2_f world_uvs[3];
    ae_vec3_f world_coords[3];
    ae_vec3_f normals[3];
    double intensity[3];

    for (size_t i = 0; i < n_faces_ae_model(model); i++)
    {
        face_ae_model(model, i, face);
        for (size_t j = 0; j < 3; j++)
        {
            vert_ae_model(model, face[j].v_i, &world_coords[j]);

            AE_V2M_RENDER(wc_m, world_coords[j]);
            AE_MATRIX_F_MULT(Z_m, Z, wc_m, 4, 1);
            AE_M2V_RENDER(world_coords[j], Z_m);

            uv_ae_model(model, face[j].uv_i, &world_uvs[j]);
            world_uvs[j].u = world_uvs[j].u * model->texture.width;
            world_uvs[j].v = world_uvs[j].v * model->texture.height;

            normal_ae_model(model, face[j].norm_i, &normals[j]);
            AE_VEC3_NORMALIZE(normals[j], normals[j], 1);
            intensity[j] = AE_VEC3_DOT(normals[j], light_dir);
        }

        triangle_ae_render(image, &model->texture, world_coords, world_uvs, intensity, zbuffer);
    }

    free(zbuffer);

    return;
}
