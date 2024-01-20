#include "aether_stack.h"

ae_stack create_ae_stack(size_t data_size)
{
    ae_stack new_stack = {
        .last = NULL,
        .data_size = data_size,
        .quant = 0};

    return new_stack;
}

uint8_t push_ae_stack(ae_stack *stack, void *par)
{
    if (stack == NULL)
        return 1;
    ae_node1w *p = malloc(sizeof(ae_node1w));
    if (p == NULL)
        return 2;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", p);
#endif // DEBUG_AE
    p->data = malloc(sizeof(stack->data_size));
    if (p->data == NULL)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", p);
#endif // DEBUG_AE
        free(p);
        return 3;
    }
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", p->data);
#endif // DEBUG_AE
    memmove(p->data, par, stack->data_size);
    if (stack->last == NULL)
    {
        stack->last = p;
        p->next = NULL;
    }
    else
    {
        p->next = stack->last;
        stack->last = p;
    }
    stack->quant++;

    return 0;
}

uint8_t pop_ae_stack(ae_stack *stack, void *par)
{
    if (stack == NULL)
        return 1;
    if (stack->quant == 0)
        return 2;
    if (stack->last == NULL)
        return 3;
    if (par != NULL)
        memmove(par, stack->last->data, stack->data_size);
    ae_node1w *p = stack->last;
    stack->last = stack->last->next;

#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p->data);
#endif // DEBUG_AE
    free(p->data);
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p);
#endif // DEBUG_AE
    free(p);

    stack->quant--;

    return 0;
}

uint8_t top_ae_stack(ae_stack *stack, void *par)
{
    if (stack == NULL)
        return 1;
    if (stack->quant == 0)
        return 2;
    if (stack->last == NULL)
        return 3;
    if (par != NULL)
        memmove(par, stack->last->data, stack->data_size);

    return 0;
}

uint8_t free_ae_stack(ae_stack *stack)
{
    if (stack == NULL)
        return 0;
    if (stack->last == NULL)
        return 0;
    ae_node1w *p1 = stack->last;
    ae_node1w *p2 = p1->next;
    while (p1 != NULL)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", p1->data);
#endif // DEBUG_AE
        free(p1->data);
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", p1);
#endif // DEBUG_AE
        free(p1);
        p1 = p2;
        if (p1 != NULL)
            p2 = p1->next;
    }

    return 0;
}
