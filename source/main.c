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


    for (size_t i = 0; i < 450; i++)
    {
        pop_back_ae_deque(&deq, NULL);
        pop_front_ae_deque(&deq, NULL);
    }
    buf = 0;
    while(!pop_front_ae_deque(&deq,&out))
    {
        printf("%llu %llu \n", buf, out);
        buf++;
    }
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

    return 0;
}
