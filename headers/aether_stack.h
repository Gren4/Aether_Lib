#ifndef __AETHER_STACK__
#define __AETHER_STACK__

#include "aether_node.h"

typedef struct
{
    ae_node1w *last;
    size_t data_size;
    size_t quant;
} ae_stack;

ae_stack create_ae_stack(size_t data_size);

uint8_t push_ae_stack(ae_stack *stack, void *par);

uint8_t pop_ae_stack(ae_stack *stack, void *par);

uint8_t top_ae_stack(ae_stack *stack, void *par);

uint8_t free_ae_stack(ae_stack *stack);

#endif // __AETHER_STACK__
