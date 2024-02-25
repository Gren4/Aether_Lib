#include "aether_lib.h"
#include "aether_render.h"
#include "aether_utils.h"

#include <time.h>

#include <stdio.h>
#include <string.h>

void print_vec(ae_vector *vec)
{
    uint8_t buf;
    size_t i = 0;
    for (i = 0; i < vec->data.quant; i++)
    {
        get_ae_vector(vec, i, &buf);
        printf("%d ", buf);
    }
    printf("\n");

    return;
}

void print_map(ae_map *map)
{

    return;
}

int comp(const void *a, const void *b)
{
    if (*(uint8_t *)a < *(uint8_t *)b)
        return -1;
    else if (*(uint8_t *)a > *(uint8_t *)b)
        return 1;
    else
        return 0;
}

typedef struct are
{
    int a;
    int b;
} are;

int main(void)
{
    clock_t tic, toc;

    tic = clock();
    ae_model model = open_ae_model("african_head.obj", "african_head_diffuse2.tga");
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    AE_TGA_C_RGBA(white, 255, 255, 255, 255);
    AE_TGA_C_RGBA(red, 255, 0, 0, 255);
    tic = clock();
    ae_tga_i image = create_ae_tga(800,800,RGBA);
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    render_model(&model, &image, &white);
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    flip_vertically_ae_tga(&image);
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    write_file_ae_tga(&image, "output.tga", false);
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    close_ae_model(&model);

    return 0;
}
