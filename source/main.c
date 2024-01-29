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
    ae_map map = create_ae_map(sizeof(size_t), 1000000, NULL);
    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    tic = clock();
    char str[80];
    buf = 0;
    size_t key = 0;
    for (size_t i = 0; i < 1000000; i++)
    {
        set_ae_map(&map, (const char *)&key, sizeof(key),&buf);
        buf++;
        key++;
    }

    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");

    key = 121;
    if (has_key_ae_map(&map, (const char *)&key, sizeof(key)))
        printf("121 bus\n");
    else
        printf("121 no bus\n");

    key = 99;
    if (has_key_ae_map(&map, (const char *)&key, sizeof(key)))
        printf("99 bus\n");
    else
        printf("99 no bus\n");

    key = 999999;
    if (has_key_ae_map(&map, (const char *)&key, sizeof(key)))
        printf("999999 bus\n");
    else
        printf("999999 no bus\n");

    key = 1999999;
    if (has_key_ae_map(&map, (const char *)&key, sizeof(key)))
        printf("1999999 bus\n");
    else
        printf("1999999 no bus\n");

    key = 0;
    for (size_t i = 0; i < 1000000; i++)
    {
        get_ae_map(&map, (const char *)&key, sizeof(key), &buf);
        if (key != buf)
            printf("key %lld contains %lld\n", key, buf);
        key++;
    }
    free_ae_map(&map);

    return 0;
}
