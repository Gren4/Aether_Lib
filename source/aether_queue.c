#include "aether_queue.h"

ae_queue create_ae_queue(size_t data_size)
{
    ae_queue new_queue = {
        .front = NULL,
        .back = NULL,
        .data_size = data_size,
        .quant = 0};

    return new_queue;
}

uint8_t push_ae_queue(ae_queue *queue, void *par)
{
    if (queue == NULL)
        return 1;
    ae_node1w *p = malloc(sizeof(ae_node1w));
    if (p == NULL)
        return 2;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", p);
#endif // DEBUG_AE
    p->data = malloc(sizeof(queue->data_size));
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
    memmove(p->data, par, queue->data_size);
    if (queue->back == NULL)
    {
        queue->front = p;
        queue->back = p;
        p->next = NULL;
    }
    else
    {
        p->next = NULL;
        queue->back->next = p;
        queue->back = p;
    }
    queue->quant++;

    return 0;
}

uint8_t pop_ae_queue(ae_queue *queue, void *par)
{
    if (queue == NULL)
        return 1;
    if (queue->quant == 0)
        return 2;
    if (queue->front == NULL)
        return 3;
    if (par != NULL)
        memmove(par, queue->front->data, queue->data_size);
    ae_node1w *p = queue->front;
    queue->front = queue->front->next;

#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p->data);
#endif // DEBUG_AE
    free(p->data);
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p);
#endif // DEBUG_AE
    free(p);

    queue->quant--;

    return 0;
}

uint8_t front_ae_queue(ae_queue *queue, void *par)
{
    if (queue == NULL)
        return 1;
    if (queue->quant == 0)
        return 2;
    if (queue->front == NULL)
        return 3;
    if (par != NULL)
        memmove(par, queue->front->data, queue->data_size);

    return 0;
}

uint8_t back_ae_queue(ae_queue *queue, void *par)
{
    if (queue == NULL)
        return 1;
    if (queue->quant == 0)
        return 2;
    if (queue->back == NULL)
        return 3;
    if (par != NULL)
        memmove(par, queue->back->data, queue->data_size);

    return 0;
}

uint8_t free_ae_queue(ae_queue *queue)
{
    if (queue == NULL)
        return 0;
    if (queue->front == NULL)
        return 0;
    ae_node1w *p1 = queue->front;
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
