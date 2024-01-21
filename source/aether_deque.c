#include "aether_deque.h"

ae_deque create_ae_deque(size_t data_size)
{
    ae_deque new_deque = {
        .pointers = init_ae_base(),
        .data_size = data_size,
        .front_i = -1,
        .back_i = -1,
        .front_block = 1,
        .back_block = 1};

    prepare_ae_deque(&new_deque);

    return new_deque;
}

void prepare_ae_deque(ae_deque *deque)
{
    if (deque == NULL)
        return;
    if (create_ae_base(&deque->pointers, sizeof(void *), AETHER_DEQUE_ADD_SIZE) != 0)
        return;
    size_t i;
    for (i = 0; i < 2; i++)
    {
        void *link_array = calloc(AETHER_LINK_ARRAY_ADD_SIZE, deque->data_size);
        if (link_array == NULL)
        {
            free_ae_base(&deque->pointers);
            return;
        }
        if (append_ae_base(&deque->pointers, sizeof(void *), AETHER_DEQUE_ADD_SIZE, &link_array) != 0)
        {
#ifdef DEBUG_AE
            printf("Freed memory at address %p\n", link_array);
#endif
            free(link_array);
            free_ae_base(&deque->pointers);
            return;
        }
#ifdef DEBUG_AE
        printf("Allocated memory at address %p\n", link_array);
#endif
    }
    return;
}

uint8_t push_front_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (deque->front_i == -1 && deque->back_i == -1)
    {
        deque->front_i = 0;
        deque->back_i = 0;
    }
    else
    {
        if (deque->front_i == 0)
        {
            if (deque->front_block == 0)
                if (create_front_array_link_ae_deque(deque) != 0)
                    return 3;
            deque->front_block--;
        }
        deque->front_i = deque->front_i == 0 ? AETHER_LINK_ARRAY_ADD_SIZE - 1 : deque->front_i - 1;
    }
    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), deque->front_block, &link_array) != 0)
        return 4;
    memmove(link_array + deque->front_i * deque->data_size, par, deque->data_size);

    return 0;
}

uint8_t push_back_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (deque->front_i == -1 && deque->back_i == -1)
    {
        deque->front_i = 0;
        deque->back_i = 0;
    }
    else
    {
        if (deque->back_i == AETHER_LINK_ARRAY_ADD_SIZE - 1)
        {
            if (deque->back_block == deque->pointers.quant - 1)
            {
                if (create_back_array_link_ae_deque(deque) != 0)
                    return 3;
            }
            deque->back_block++;
        }
        deque->back_i = (deque->back_i + 1) % AETHER_LINK_ARRAY_ADD_SIZE;
    }
    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), deque->back_block, &link_array) != 0)
        return 4;
    memmove(link_array + deque->back_i * deque->data_size, par, deque->data_size);

    return 0;
}

uint8_t pop_front_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (deque->front_i == -1 && deque->back_i == -1)
        return 3;
    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), deque->front_block, &link_array) != 0)
        return 4;
    if (par != NULL)
        memmove(par, link_array + deque->front_i * deque->data_size, deque->data_size);
    if (deque->front_block == deque->back_block)
    {
        if (deque->front_i + 1 > deque->back_i)
        {
            deque->front_i = -1;
            deque->back_i = -1;
        }
        else
        {
            deque->front_i++;
        }
    }
    else
    {
        if (deque->front_i == AETHER_LINK_ARRAY_ADD_SIZE - 1)
        {
            deque->front_block++;
            if (deque->front_block >= 2)
                if (delete_front_array_link_ae_deque(deque) != 0)
                    return 5;
        }
        deque->front_i = (deque->front_i + 1) % AETHER_LINK_ARRAY_ADD_SIZE;
    }

    return 0;
}

uint8_t pop_back_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (deque->front_i == -1 && deque->back_i == -1)
        return 3;
    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), deque->back_block, &link_array) != 0)
        return 4;
    if (par != NULL)
        memmove(par, link_array + deque->back_i * deque->data_size, deque->data_size);
    if (deque->front_block == deque->back_block)
    {
        if (deque->front_i > deque->back_i - 1)
        {
            deque->front_i = -1;
            deque->back_i = -1;
        }
        else
        {
            deque->back_i--;
        }
    }
    else
    {
        if (deque->back_i == 0)
        {
            deque->back_block--;
            if (deque->back_block <= deque->pointers.quant - 3)
                if (delete_back_array_link_ae_deque(deque) != 0)
                    return 5;
        }
        deque->back_i = deque->back_i == 0 ? AETHER_LINK_ARRAY_ADD_SIZE - 1 : deque->back_i - 1;
    }

    return 0;
}

uint8_t create_front_array_link_ae_deque(ae_deque *deque)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    void *link_array = calloc(AETHER_LINK_ARRAY_ADD_SIZE, deque->data_size);
    if (link_array == NULL)
        return 3;
    if (insert_ae_base(&deque->pointers, sizeof(void *), AETHER_DEQUE_ADD_SIZE, 0, &link_array) != 0)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", link_array);
#endif
        free(link_array);
        return 4;
    }
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", link_array);
#endif
    deque->front_block++;
    deque->back_block++;

    return 0;
}

uint8_t create_back_array_link_ae_deque(ae_deque *deque)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    void *link_array = calloc(AETHER_LINK_ARRAY_ADD_SIZE, deque->data_size);
    if (link_array == NULL)
        return 3;
    if (append_ae_base(&deque->pointers, sizeof(void *), AETHER_DEQUE_ADD_SIZE, &link_array) != 0)
    {
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", link_array);
#endif
        free(link_array);
        return 4;
    }
#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", link_array);
#endif

    return 0;
}

uint8_t delete_front_array_link_ae_deque(ae_deque *deque)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    void *link_array;
    if (delete_ae_base(&deque->pointers, sizeof(void *), AETHER_DEQUE_ADD_SIZE, 0, &link_array) != 0)
    {
        return 3;
    }
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", link_array);
#endif
    free(link_array);
    deque->front_block--;
    deque->back_block--;

    return 0;
}

uint8_t delete_back_array_link_ae_deque(ae_deque *deque)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    void *link_array;
    if (delete_ae_base(&deque->pointers, sizeof(void *), AETHER_DEQUE_ADD_SIZE, deque->pointers.quant - 1, &link_array) != 0)
    {
        return 3;
    }
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", link_array);
#endif
    free(link_array);

    return 0;
}

uint8_t front_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (deque->front_i == -1 && deque->back_i == -1)
        return 3;
    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), deque->front_block, &link_array) != 0)
        return 4;
    if (par == NULL)
        return 5;
    memmove(par, link_array + deque->front_i * deque->data_size, deque->data_size);

    return 0;
}

uint8_t back_ae_deque(ae_deque *deque, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (deque->front_i == -1 && deque->back_i == -1)
        return 3;
    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), deque->back_block, &link_array) != 0)
        return 4;
    if (par == NULL)
        return 5;
    memmove(par, link_array + deque->back_i * deque->data_size, deque->data_size);

    return 0;
}

uint8_t get_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (par == NULL)
        return 3;
    if (i < 0 || i > (((deque->back_block - deque->front_block) - 1) * AETHER_LINK_ARRAY_ADD_SIZE) + (deque->back_i + (AETHER_LINK_ARRAY_ADD_SIZE - deque->front_i)))
        return 4;

    size_t link_i = (i + deque->front_i) / AETHER_LINK_ARRAY_ADD_SIZE;
    i = (i + deque->front_i) - AETHER_LINK_ARRAY_ADD_SIZE * link_i;
    link_i += deque->front_block;

    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), link_i, &link_array) != 0)
        return 5;

    memmove(par, link_array + i * deque->data_size, deque->data_size);
    return 0;
}

uint8_t set_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (par == NULL)
        return 3;
    if (i < 0 || i > (((deque->back_block - deque->front_block) - 1) * AETHER_LINK_ARRAY_ADD_SIZE) + (deque->back_i + (AETHER_LINK_ARRAY_ADD_SIZE - deque->front_i)))
        return 4;

    size_t link_i = (i + deque->front_i) / AETHER_LINK_ARRAY_ADD_SIZE;
    i = (i + deque->front_i) - AETHER_LINK_ARRAY_ADD_SIZE * link_i;
    link_i += deque->front_block;

    void *link_array;
    if (get_ae_base(&deque->pointers, sizeof(void *), link_i, &link_array) != 0)
        return 5;

    memmove(link_array + i * deque->data_size, par, deque->data_size);
    return 0;
}

uint8_t insert_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (par == NULL)
        return 3;
    size_t quant = (((deque->back_block - deque->front_block) - 1) * AETHER_LINK_ARRAY_ADD_SIZE) + (deque->back_i + 1 + (AETHER_LINK_ARRAY_ADD_SIZE - deque->front_i));
    if (i < 0 || i > quant)
        return 4;

    if (i == 0)
    {
        if (push_front_ae_deque(deque, par) != 0)
            return 5;
        return 0;
    }
    else if (i == quant - 1)
    {
        if (push_back_ae_deque(deque, par) != 0)
            return 5;
        return 0;
    }

    size_t link_f_i = (i + deque->front_i) / AETHER_LINK_ARRAY_ADD_SIZE;
    size_t k = (i + deque->front_i) - AETHER_LINK_ARRAY_ADD_SIZE * link_f_i;
    link_f_i += deque->front_block;

    if (link_f_i - deque->front_block >= deque->back_block - link_f_i)
    {
        size_t j = deque->back_block;
        void *link_array_1;
        void *link_array_2;
        void *p = malloc(deque->data_size);

        if (p == NULL)
            return 6;
        if (get_ae_base(&deque->pointers, sizeof(void *), deque->back_block, &link_array_1) != 0)
            return 7;
        if (back_ae_deque(deque, p) != 0)
            return 8;
        memmove(link_array_1 + deque->data_size, link_array_1, deque->back_i * deque->data_size);
        for (; j > link_f_i; j--)
        {
            if (get_ae_base(&deque->pointers, sizeof(void *), j - 1, &link_array_2) != 0)
                return 9;

            memmove(link_array_1, link_array_2 + (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size, deque->data_size);
            if (j - 1 == link_f_i)
                memmove(link_array_2 + (k + 1) * deque->data_size, link_array_2 + k * deque->data_size, (AETHER_LINK_ARRAY_ADD_SIZE - k - 1) * deque->data_size);
            else
                memmove(link_array_2 + deque->data_size, link_array_2, (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size);

            link_array_1 = link_array_2;
        }

        memmove(link_array_1 + k * deque->data_size, par, deque->data_size);
        if (push_back_ae_deque(deque, p) != 0)
            return 10;
        free(p);
    }
    else
    {
        link_f_i = (i - 1 + deque->front_i) / AETHER_LINK_ARRAY_ADD_SIZE;
        k = (i - 1 + deque->front_i) - AETHER_LINK_ARRAY_ADD_SIZE * link_f_i;
        link_f_i += deque->front_block;

        size_t j = deque->front_block;
        void *link_array_1;
        void *link_array_2;
        void *p = malloc(deque->data_size);

        if (p == NULL)
            return 6;
        if (get_ae_base(&deque->pointers, sizeof(void *), deque->front_block, &link_array_1) != 0)
            return 7;
        if (front_ae_deque(deque, p) != 0)
            return 8;
        if (link_f_i != j)
            memmove(link_array_1 + deque->front_i * deque->data_size, link_array_1 + (deque->front_i + 1) * deque->data_size, (AETHER_LINK_ARRAY_ADD_SIZE - deque->front_i - 1) * deque->data_size);
        else
            memmove(link_array_1 + deque->front_i * deque->data_size, link_array_1 + (deque->front_i + 1) * deque->data_size, (AETHER_LINK_ARRAY_ADD_SIZE - k - 1) * deque->data_size);
        for (; j < link_f_i; j++)
        {
            if (get_ae_base(&deque->pointers, sizeof(void *), j + 1, &link_array_2) != 0)
                return 9;

            memmove(link_array_1 + (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size, link_array_2, deque->data_size);
            if (j + 1 == link_f_i)
                memmove(link_array_2, link_array_2 + deque->data_size, k * deque->data_size);
            else
                memmove(link_array_2, link_array_2 + deque->data_size, (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size);

            link_array_1 = link_array_2;
        }

        memmove(link_array_1 + k * deque->data_size, par, deque->data_size);
        if (push_front_ae_deque(deque, p) != 0)
            return 10;
        free(p);
    }
    return 0;
}

uint8_t delete_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (deque == NULL)
        return 1;
    if (deque->pointers.memory == NULL)
        return 2;
    if (par == NULL)
        return 3;
    size_t quant = (((deque->back_block - deque->front_block) - 1) * AETHER_LINK_ARRAY_ADD_SIZE) + (deque->back_i + 1 + (AETHER_LINK_ARRAY_ADD_SIZE - deque->front_i));
    if (i < 0 || i > quant)
        return 4;

    if (i == 0)
    {
        if (pop_front_ae_deque(deque, par) != 0)
            return 5;
        return 0;
    }
    else if (i == quant - 1)
    {
        if (pop_back_ae_deque(deque, par) != 0)
            return 5;
        return 0;
    }

    size_t link_f_i = (i + deque->front_i) / AETHER_LINK_ARRAY_ADD_SIZE;
    size_t k = (i + deque->front_i) - AETHER_LINK_ARRAY_ADD_SIZE * link_f_i;
    link_f_i += deque->front_block;

    if (par != NULL)
    {
        void *link_array;
        if (get_ae_base(&deque->pointers, sizeof(void *), link_f_i, &link_array) != 0)
            return 6;

        memmove(par, link_array + k * deque->data_size, deque->data_size);
    }

    if (link_f_i - deque->front_block >= deque->back_block - link_f_i)
    {
        size_t j = link_f_i;
        void *link_array_1;
        void *link_array_2;
        if (get_ae_base(&deque->pointers, sizeof(void *), j, &link_array_1) != 0)
            return 7;
        memmove(link_array_1 + k * deque->data_size, link_array_1 + (k + 1) * deque->data_size, (AETHER_LINK_ARRAY_ADD_SIZE - 1 - k) * deque->data_size);
        for (; j < deque->back_block; j++)
        {
            if (get_ae_base(&deque->pointers, sizeof(void *), j + 1, &link_array_2) != 0)
                return 8;
            memmove(link_array_1 + (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size, link_array_2, deque->data_size);
            memmove(link_array_2, link_array_2 + deque->data_size, (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size);
            link_array_1 = link_array_2;
        }
        if (pop_back_ae_deque(deque, NULL) != 0)
            return 9;
    }
    else
    {
        size_t j = link_f_i;
        void *link_array_1;
        void *link_array_2;
        if (get_ae_base(&deque->pointers, sizeof(void *), j, &link_array_1) != 0)
            return 7;
        memmove(link_array_1 + deque->data_size, link_array_1, k * deque->data_size);
        for (; j > deque->front_block; j--)
        {
            if (get_ae_base(&deque->pointers, sizeof(void *), j - 1, &link_array_2) != 0)
                return 8;
            memmove(link_array_1, link_array_2 + (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size, deque->data_size);
            memmove(link_array_2 + deque->data_size, link_array_2, (AETHER_LINK_ARRAY_ADD_SIZE - 1) * deque->data_size);
            link_array_1 = link_array_2;
        }
        if (pop_front_ae_deque(deque, NULL) != 0)
            return 9;
    }
    return 0;
}

uint8_t free_ae_deque(ae_deque *deque)
{
    if (deque == NULL)
        return 0;
    if (deque->pointers.memory == NULL)
        return 0;
    size_t i;
    for (i = 0; i < deque->pointers.quant; i++)
    {
        void *link_array;
        if (get_ae_base(&deque->pointers, sizeof(void *), i, &link_array) != 0)
            return 1;
#ifdef DEBUG_AE
        printf("Freed memory at address %p\n", link_array);
#endif
        free(link_array);
    }
    free_ae_base(&deque->pointers);

    return 0;
}
