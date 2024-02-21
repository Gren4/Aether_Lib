#include "aether_render.h"
#include "aether_model.h"
#include "aether_utils.h"
#include <math.h>

void line_ae_render(int32_t x0, int32_t y0, int32_t x1, int32_t y1, ae_tga_i *image, ae_tga_c *color)
{
    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1))
    {
        swap_ae(int32_t, x0, y0);
        swap_ae(int32_t, x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        swap_ae(int32_t, x0, x1);
        swap_ae(int32_t, y0, y1);
    }
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t derror2 = abs(dy) * 2;
    int32_t error2 = 0;
    int32_t y = y0;
    if (steep)
    {
        for (int32_t x = x0; x <= x1; x++)
        {
            set_ae_tga(image, y, x, color);
            error2 += derror2;
            if (error2 > dx)
            {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx * 2;
            }
        }
    }
    else
    {
        for (int32_t x = x0; x <= x1; x++)
        {
            set_ae_tga(image, x, y, color);
            error2 += derror2;
            if (error2 > dx)
            {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx * 2;
            }
        }
    }

    return;
}

void render_model(ae_model *model, ae_tga_i *image, ae_tga_c * color)
{
    for (int i = 0; i < n_faces_ae_model(model); i++)
    {
        ae_vector face = face_ae_model(model, i);
        size_t face_i = 0;
        for (int j=0; j<3; j++) {
            get_ae_vector(&face, j, &face_i);
            ae_vec3_f v0 = vert_ae_model(model, face_i);
            get_ae_vector(&face, (j+1)%3, &face_i);
            ae_vec3_f v1 =vert_ae_model(model, face_i);
            int x0 = (v0.x+1.)*image->width/2.;
            int y0 = (v0.y+1.)*image->height/2.;
            int x1 = (v1.x+1.)*image->width/2.;
            int y1 = (v1.y+1.)*image->height/2.;
            line_ae_render(x0, y0, x1, y1, image, color);
        }
    }
    return;
}
