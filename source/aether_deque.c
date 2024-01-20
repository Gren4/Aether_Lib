#include "aether_deque.h"

ae_deque create_ae_deque(size_t data_size)
{
    ae_deque new_deque = {
        .front = NULL,
        .back = NULL,
        .data_size = data_size,
        .quant = 0};

    return new_deque;
}

uint8_t push_front_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    ae_node2w *p = malloc(sizeof(ae_node2w));
    if (p == NULL)
        return 2;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", p);
#endif // DEBUG_AE
    p->data = malloc(sizeof(deque->data_size));
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
    memmove(p->data, par, deque->data_size);
    if (deque->front == NULL)
    {
        deque->front = p;
        deque->back = p;
        p->next = NULL;
    }
    else
    {
        p->prev = NULL;
        p->next = deque->front;
        deque->front->prev = p;
        deque->front = p;
    }
    deque->quant++;

    return 0;
}

uint8_t push_back_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    ae_node2w *p = malloc(sizeof(ae_node2w));
    if (p == NULL)
        return 2;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", p);
#endif // DEBUG_AE
    p->data = malloc(sizeof(deque->data_size));
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
    memmove(p->data, par, deque->data_size);
    if (deque->back == NULL)
    {
        deque->front = p;
        deque->back = p;
        p->next = NULL;
    }
    else
    {
        p->prev = deque->back;
        p->next = NULL;
        deque->back->next = p;
        deque->back = p;
    }
    deque->quant++;

    return 0;
}

uint8_t pop_front_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->quant == 0)
        return 2;
    if (deque->front == NULL)
        return 3;
    if (par != NULL)
        memmove(par, deque->front->data, deque->data_size);
    ae_node2w *p = deque->front;
    deque->front = deque->front->next;
    if (deque->front != NULL)
        deque->front->prev = NULL;

#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p->data);
#endif // DEBUG_AE
    free(p->data);
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p);
#endif // DEBUG_AE
    free(p);

    deque->quant--;

    return 0;
}

uint8_t pop_back_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->quant == 0)
        return 2;
    if (deque->front == NULL)
        return 3;
    if (par != NULL)
        memmove(par, deque->back->data, deque->data_size);
    ae_node2w *p = deque->back;
    deque->back = deque->back->prev;
    if (deque->back != NULL)
        deque->back->next = NULL;

#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p->data);
#endif // DEBUG_AE
    free(p->data);
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p);
#endif // DEBUG_AE
    free(p);

    deque->quant--;

    return 0;
}

uint8_t front_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->quant == 0)
        return 2;
    if (deque->front == NULL)
        return 3;
    if (par != NULL)
        memmove(par, deque->front->data, deque->data_size);

    return 0;
}

uint8_t back_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->quant == 0)
        return 2;
    if (deque->back == NULL)
        return 3;
    if (par != NULL)
        memmove(par, deque->back->data, deque->data_size);

    return 0;
}

uint8_t get_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->quant == 0)
        return 2;
    if (i < 0 || i >= deque->quant)
        return 3;
    size_t counter = 0;
    ae_node2w *p;
    if (i <= deque->quant / 2)
    {
        counter = 0;
        p = deque->front;
        while (counter != i)
        {
            p = p->next;
            counter++;
        }
    }
    else
    {
        counter = deque->quant - 1;
        p = deque->back;
        while (counter != i)
        {
            p = p->prev;
            counter--;
        }
    }
    if (par != NULL)
        memmove(par, p->data, deque->data_size);

    return 0;
}

uint8_t insert_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->quant == 0)
        return 2;
    if (par == NULL)
        return 3;
    if (i < 0 || i >= deque->quant)
        return 4;
    size_t counter = 0;
    ae_node2w *p;
    if (i <= deque->quant / 2)
    {
        counter = 0;
        p = deque->front;
        while (counter != i)
        {
            p = p->next;
            counter++;
        }
    }
    else
    {
        counter = deque->quant - 1;
        p = deque->back;
        while (counter != i)
        {
            p = p->prev;
            counter--;
        }
    }
    ae_node2w *p_new = malloc(sizeof(ae_node2w));
    if (p_new == NULL)
        return 5;
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", p_new);
#endif // DEBUG_AE
    p_new->data = malloc(sizeof(deque->data_size));
    if (p_new->data == NULL)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", p_new);
#endif // DEBUG_AE
        free(p_new);
        return 6;
    }
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", p_new->data);
#endif // DEBUG_AE
    memmove(p_new->data, par, deque->data_size);

    p_new->next = p;
    p_new->prev = p->prev;
    ((ae_node2w *)p->prev)->next = p_new;
    p->prev = p_new;

    deque->quant++;

    return 0;
}

uint8_t delete_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->quant == 0)
        return 2;
    if (par == NULL)
        return 3;
    if (i < 0 || i >= deque->quant)
        return 4;
    size_t counter = 0;
    ae_node2w *p;
    if (i <= deque->quant / 2)
    {
        counter = 0;
        p = deque->front;
        while (counter != i)
        {
            p = p->next;
            counter++;
        }
    }
    else
    {
        counter = deque->quant - 1;
        p = deque->back;
        while (counter != i)
        {
            p = p->prev;
            counter--;
        }
    }

    ((ae_node2w *)p->prev)->next = p->next;
    ((ae_node2w *)p->next)->prev = p->prev;
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p->data);
#endif // DEBUG_AE
    free(p->data);
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", p);
#endif // DEBUG_AE
    free(p);

    deque->quant--;

    return 0;
}

uint8_t free_ae_deque(ae_deque *deque)
{
    if (deque == NULL)
        return 0;
    if (deque->front == NULL)
        return 0;
    ae_node2w *p1 = deque->front;
    ae_node2w *p2 = p1->next;
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
