#include "aether_render.h"

#include <time.h>

#include <stdio.h>
#include <string.h>

int main(void)
{
    clock_t tic, toc;

    tic = clock();
    ae_model model = open_ae_model("african_head.obj", "african_head_diffuse.tga", "african_head_nm_tangent.tga", "african_head_spec.tga");
    toc = clock();
    printf("Open model execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    ae_tga_i image = create_ae_tga(2880, 1920, RGBA);
    toc = clock();
    printf("TGA create execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    render_model(&model, &image);
    toc = clock();
    printf("Render execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    flip_vertically_ae_tga(&image);
    toc = clock();
    printf("Flip TGA execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    write_file_ae_tga(&image, "output.tga", false);
    toc = clock();
    printf("Image output execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    close_ae_model(&model);

    return 0;
}
