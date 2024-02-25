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

        size_t buf = 1;
    size_t out = 0;

    ae_deque deq = create_ae_deque(sizeof(size_t));

    tic = clock();
    for (size_t i = 0; i < 1000; i++)
    {
        push_back_ae_deque(&deq, &buf);
        buf++;
    }
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");
    buf = 0;
    tic = clock();

    for (size_t i = 0; i < 450; i++)
    {
        pop_front_ae_deque(&deq, &buf);
        pop_back_ae_deque(&deq, &buf);
    }

    while(!pop_front_ae_deque(&deq,&out))
    {
        printf("%llu %llu \n", buf, out);
        buf++;
    }
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    free_ae_deque(&deq);

    ae_vector vec = create_ae_vector(sizeof(size_t), 0);

    tic = clock();
    for (size_t i = 0; i < 1000000; i++)
    {
        append_ae_vector(&vec, &buf);
        buf++;
    }
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");
    free_ae_vector(&vec);



    tic = clock();
    ae_model model = open_ae_model("african_head.obj", "african_head_diffuse1.tga");
    toc = clock();
    printf("Open model execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    AE_TGA_C_RGBA(white, 255, 255, 255, 255);
    AE_TGA_C_RGBA(red, 255, 0, 0, 255);
    tic = clock();
    ae_tga_i image = create_ae_tga(4096,4096,RGBA);
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
