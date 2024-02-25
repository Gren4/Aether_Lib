#include "aether_render.h"
#include "aether_model.h"
#include "aether_utils.h"
#include <math.h>
#include <float.h>

void line_ae_render(ae_vec2_i p1, ae_vec2_i p2, ae_tga_i *image, ae_tga_c *color)
{
    bool steep = false;
    if (abs(p1.x - p2.x) < abs(p1.y - p2.y))
    {
        swap_ae(int32_t, p1.x, p1.y);
        swap_ae(int32_t, p2.x, p2.y);
        steep = true;
    }
    if (p1.x > p2.x)
    {
        swap_ae(ae_vec2_i, p1, p2);
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

void triangle_ae_render(ae_vec3_f p1, ae_vec3_f p2, ae_vec3_f p3, double *zbuffer, ae_tga_i *image, ae_tga_c *color)
{
    if (p1.y == p2.y && p1.y == p3.y)
        return;

    if (p1.y > p2.y)
        swap_ae(ae_vec3_f, p1, p2);
    if (p1.y > p3.y)
        swap_ae(ae_vec3_f, p1, p3);
    if (p2.y > p3.y)
        swap_ae(ae_vec3_f, p2, p3);

    double alpha;
    double beta;
    ae_vec3_f A;
    ae_vec3_f B;
    size_t i;
    size_t j;

    int32_t p1_y = (int32_t)p1.y;
    int32_t p2_y = (int32_t)p2.y;
    int32_t p3_y = (int32_t)p3.y;

    int32_t total_height = p3_y - p1_y;
    total_height = total_height == 0 ? 1 : total_height;
    int32_t segment_height = p2_y - p1_y;
    segment_height = segment_height == 0 ? 1 : segment_height;
    double depth;
    for (i = (p1_y >= 0 ? p1_y : 0); i <= (p2_y < image->height ? p2_y : image->height - 1); i++)
    {
        alpha = (double)(i - p1_y) / total_height;
        beta = (double)(i - p1_y) / segment_height;
        AE_VEC3_DIF(A, p3, p1);
        AE_VEC3_F_MULT(A, A, alpha);
        AE_VEC3_SUM(A, A, p1);
        AE_VEC3_DIF(B, p2, p1);
        AE_VEC3_F_MULT(B, B, beta);
        AE_VEC3_SUM(B, B, p1);
        if (A.x > B.x)
            swap_ae(ae_vec3_f, A, B);
        depth = (p1.z * (A.z + B.z) / 2) + (p2.z * (A.z + B.z) / 2) + (p3.z * (A.z + B.z) / 2);
        for (j = A.x; j <= (B.x < image->width ? B.x : image->width - 1); j++)
        {
            if (zbuffer[j + i * image->width] < depth)
            {
                set_ae_tga(image, j, i, color);
                zbuffer[j + i * image->width] = depth;
            }
        }
    }

    segment_height = p3_y - p2_y;
    for (i = (p2_y >= 0 ? p2_y : 0); i <= (p3_y < image->height ? p3_y : image->height - 1); i++)
    {
        alpha = (double)(i - p1_y) / total_height;
        beta = (double)(i - p2_y) / segment_height;
        AE_VEC3_DIF(A, p3, p1);
        AE_VEC3_F_MULT(A, A, alpha);
        AE_VEC3_SUM(A, A, p1);
        AE_VEC3_DIF(B, p3, p2);
        AE_VEC3_F_MULT(B, B, beta);
        AE_VEC3_SUM(B, B, p2);
        if (A.x > B.x)
            swap_ae(ae_vec3_f, A, B);
        depth = (p1.z * (A.z + B.z) / 2) + (p2.z * (A.z + B.z) / 2) + (p3.z * (A.z + B.z) / 2);
        for (j = A.x; j <= (B.x < image->width ? B.x : image->width - 1); j++)
        {
            if (zbuffer[j + i * image->width] < depth)
            {
                set_ae_tga(image, j, i, color);
                zbuffer[j + i * image->width] = depth;
            }
        }
    }

    return;
}


void render_model(ae_model *model, ae_tga_i *image, ae_tga_c *color)
{
    double* zbuffer = (double*)(calloc(image->width * image->height, sizeof(double)));
    for (size_t i = 0; i < image->width * image->height; i++)
    {
        zbuffer[i] = -DBL_MAX;
    }
    ae_vec3_f light_dir = {.x = 0.0, .y = 0.0, .z = -1.0};
    for (int i = 0; i < n_faces_ae_model(model); i++)
    {
        ae_vector face = face_ae_model(model, i);
        ae_face face_data;
        ae_vec3_f screen_coords[3];
        ae_vec3_f world_coords[3];
        for (int j = 0; j < 3; j++)
        {
            get_ae_vector(&face, j, &face_data);
            world_coords[j] = vert_ae_model(model, face_data.v_i);
            screen_coords[j].x = (world_coords[j].x + 1.0) * image->width / 2;
            screen_coords[j].y = (world_coords[j].y + 1.0) * image->height / 2;
            screen_coords[j].z = world_coords[j].z;
        }
        ae_vec3_f n;
        {
            ae_vec3_f n1, n2;
            AE_VEC3_DIF(n1, world_coords[1], world_coords[0]);
            AE_VEC3_DIF(n2, world_coords[2], world_coords[0]);
            AE_VEC3_VEC_MULT(n, n2, n1);
        }
        AE_VEC3_NORMALIZE(n, n, 1.0);
        double intensity = AE_VEC3_MULT(n, light_dir);
        if (intensity > 0.0)
        {
            ae_tga_c new_color = *color;
            new_color.r *= intensity;
            new_color.g *= intensity;
            new_color.b *= intensity;
            triangle_ae_render(screen_coords[0], screen_coords[1], screen_coords[2], zbuffer, image, &new_color);
        }
    }

    free(zbuffer);

    return;
}
