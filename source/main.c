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
    uint8_t buf;
    const char *key;
    size_t i = 0;

    ae_vector keys = get_keys_ae_map(map);

    for (i = 0; i < keys.data.quant; i++)
    {
        get_ae_vector(&keys, i, &key);
        get_ae_map(map, key, &buf);
        printf("%s is %d\n", key, buf);
    }
    free_ae_vector(&keys);
    printf("\n");

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

    uint8_t buf = 0;

    ae_map map = create_ae_map(sizeof(uint8_t), NULL);
    printf("Append\n");
    char str[400] = {0};
    tic = clock();

    for (int i = 0; i < 255; i++)
    {
        sprintf(&str[0],"%d", i);
        set_ae_map(&map, &str[0], &buf);
        buf += 1;
    }

    toc = clock();
    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("\n");
    print_map(&map);

    free_ae_map(&map);

    return 0;
}
