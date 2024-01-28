#include "aether_lib.h"
#include <time.h>

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

int main(void)
{
    clock_t tic, toc;

    size_t buf = 1;

    ae_vector vec = create_ae_vector(sizeof(size_t), 0);
    tic = clock();
    for (size_t i = 0; i < 1000000; i++)
    {
        append_ae_vector(&vec, &buf);
        buf++;
    }
    toc = clock();

    for (size_t i = 0; i < 1000000; i++)
    {
        get_ae_vector(&vec, i, &buf);
        //printf("%lld ", buf);
    }
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    free_ae_vector(&vec);

    tic = clock();
    ae_map map = create_ae_map(sizeof(size_t), 0, NULL);
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    tic = clock();
    char str[80];
    buf = 0;
    size_t key = 0;
    for (size_t i = 0; i < 100; i++)
    {
        sprintf(str, "%lld", i);
        set_ae_map(&map, (const char *)&str, strlen(str),&buf);
        buf++;
        key++;
    }

    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    sprintf(str, "%lld", 121);
    delete_ae_map(&map, (const char *)&str, strlen(str), &buf);

    sprintf(str, "%lld", 121);
    if (has_key_ae_map(&map, (const char *)&str, strlen(str)))
        printf("121 bus\n");
    else
        printf("121 no bus\n");

    sprintf(str, "%lld", 99);
    if (has_key_ae_map(&map, (const char *)&str, strlen(str)))
        printf("99 bus\n");
    else
        printf("99 no bus\n");

    free_ae_map(&map);

    return 0;
}
