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

    uint8_t buf;
    ae_vector vec = create_ae_vector(sizeof(uint8_t));

    ae_vector bec = create_ae_vector(sizeof(uint8_t));

    for (buf = 1; buf < 6; buf++)
        append_ae_vector(&vec, &buf);

    for (buf = 20; buf > 5; buf--)
        append_ae_vector(&bec, &buf);

    insert_vector_ae_vector(&vec, &bec, 2, 2, 10);

    print_vec(&vec);
    print_vec(&bec);
    free_ae_vector(&vec);
    free_ae_vector(&bec);
    printf("\n");

    ae_stack stack = create_ae_stack(sizeof(uint8_t));

    for (buf = 1; buf <= 4; buf++)
        push_ae_stack(&stack, &buf);
    top_ae_stack(&stack, &buf);
    printf("Element %d at top\n", buf);

    while (stack.quant != 0)
    {
        pop_ae_stack(&stack, &buf);
        printf("Popped element %d\n", buf);
    }
    free_ae_stack(&stack);
    printf("\n");

    ae_queue queue = create_ae_queue(sizeof(uint8_t));
    for (buf = 1; buf <= 4; buf++)
        push_ae_queue(&queue, &buf);

    front_ae_queue(&queue, &buf);
    printf("Element %d at front\n", buf);

    while (queue.quant != 0)
    {
        pop_ae_queue(&queue, &buf);
        printf("Popped element %d\n", buf);
    }
    free_ae_queue(&queue);
    printf("\n");

    ae_deque deque = create_ae_deque(sizeof(uint8_t));
    for (buf = 1; buf <= 5; buf++)
        push_back_ae_deque(&deque, &buf);
    for (buf = 5; buf <= 10; buf++)
        push_front_ae_deque(&deque, &buf);

    while(deque.quant != 0)
    {
        pop_front_ae_deque(&deque, &buf);
        printf("Popped element %d\n", buf);
    }
    free_ae_deque(&deque);
    printf("\n");

    clock_t toc = clock();

    printf("Execution time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    return 0;
}
