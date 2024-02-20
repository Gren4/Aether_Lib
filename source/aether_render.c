#include "aether_render.h"
#include "aether_utils.h"

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
