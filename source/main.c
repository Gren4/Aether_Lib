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
    double buf;
    const char *key;
    size_t i = 0;

    ae_vector keys = get_keys_ae_map(map);

    for (i = 0; i < keys.data.quant; i++)
    {
        get_ae_vector(&keys, i, &key);
        get_ae_map(map, key, &buf);
        printf("%s is %f\n", key, buf);
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
    clock_t tic = clock();
    uint8_t buf = 1;

    ae_deque deq = create_ae_deque(sizeof(uint8_t));

    for (buf = 1; buf <= 30; buf++)
    {
        push_front_ae_deque(&deq, &buf);
    }

    buf = 13;
    insert_ae_deque(&deq, 25, &buf);
    size_t i = 0;
    // pop_front_ae_deque(&deq, NULL);
    // pop_back_ae_deque(&deq, NULL);
    while (get_ae_deque(&deq, i, &buf) == 0)
    {
        printf("%lld %d\n", i, buf);
        i++;
    }

    free_ae_deque(&deq);

    clock_t toc = clock();

    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    return 0;
}
