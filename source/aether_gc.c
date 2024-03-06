#include "aether_vector.h"
#include "aether_gc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct ae_garbage_
{
    void *data;
    size_t stack_idx;
};

static size_t ae_stack_idx;
static ae_vector ae_gc;

void init_ae_gc(void)
{
    ae_stack_idx = 1;
    create_gc_ae_vector(&ae_gc, sizeof(ae_garbage));
    return;
}

size_t append_ae_gc(void *data)
{
    ae_garbage garbage = {.data = data, .stack_idx = ae_stack_idx};
    append_ae_vector(&ae_gc, (void *)(&garbage));
    return ae_gc.data.quant - 1;
}

void update_ae_gc(size_t i, void *data)
{
    ae_garbage *garbage;
    get_pointer_ae_vector(&ae_gc, i, (void**)&garbage);
    garbage->data = data;

    return;
}

void remove_ae_gc(size_t i)
{
    delete_ae_vector(&ae_gc, i, NULL);

    return;
}

void on_enter_ae_gc(void)
{
    ae_stack_idx++;
    return;
}

void on_return_ae_gc(void)
{
    ae_garbage garbage;
    while (ae_gc.data.quant != 0)
    {
        get_ae_vector(&ae_gc, ae_gc.data.quant - 1, &garbage);

        if (garbage.stack_idx == ae_stack_idx)
        {
            pop_ae_vector(&ae_gc, NULL);
#ifdef DEBUG_AE
            printf("Freed memory at address %p\n", garbage.data);
#endif
            free(garbage.data);
        }
        else
        {
            break;
        }
    }
    ae_stack_idx--;
    return;
}
