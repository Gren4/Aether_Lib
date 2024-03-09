#include "aether_deque.h"
#include <string.h>
#include <assert.h>

const size_t pointer_size = sizeof(size_t);

extern inline size_t quant_ae_deq(const ae_deq *const deque);

ae_deq create_ae_deq(size_t data_size)
{
    ae_deq new_deque = {
        .pointers = create_ae_base(&pointer_size, AETHER_DEQUE_ADD_SIZE),
        .blocks = create_max_size_ae_base(&data_size, AETHER_DEQUE_ADD_SIZE * AETHER_BLOCKS_ADD_SIZE),
        .data_size = data_size,
        .front_i = -1,
        .back_i = -1,
        .front_block = AETHER_DEQUE_ADD_SIZE / 2,
        .back_block = AETHER_DEQUE_ADD_SIZE / 2};

    for (size_t i = 0; i < AETHER_DEQUE_ADD_SIZE; i++)
    {
        *(int *)append_ae_base(&new_deque.pointers, &pointer_size) = i;
    }

    return new_deque;
}

void free_ae_deq(ae_deq *const deque)
{
    if (deque->pointers.memory != NULL)
        free_ae_base(&deque->pointers);
    if (deque->blocks.memory != NULL)
        free_ae_base(&deque->blocks);

    return;
}

static inline void reorganize_ae_deq(ae_deq *deque)
{
    if (deque->pointers.max_quant >= 16)
    {
        if (deque->front_block >= (deque->pointers.max_quant * 3) / 4)
        {
            size_t num_blocks = deque->back_block - deque->front_block + 1;

            size_t center = deque->pointers.max_quant / 2 - num_blocks / 2;

            for (size_t i = 0; i < num_blocks; i++)
            {
                set_base_ae_base(&deque->blocks,
                                 &deque->blocks,
                                 &deque->data_size,
                                 *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i + center) * deque->data_size,
                                 *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i + deque->front_block) * deque->data_size,
                                 AETHER_BLOCKS_ADD_SIZE);
            }

            deque->front_block = center;
            deque->back_block = deque->front_block + num_blocks - 1;
        }
        else if (deque->back_block <= (deque->pointers.max_quant) / 4)
        {
            size_t num_blocks = deque->back_block - deque->front_block + 1;

            size_t center = deque->pointers.max_quant / 2 + num_blocks / 2 - 1;

            for (size_t i = 0; i < num_blocks; i++)
            {
                set_base_ae_base(&deque->blocks,
                                 &deque->blocks,
                                 &deque->data_size,
                                 *(size_t *)get_ae_base(&deque->pointers, &pointer_size, center - i) * deque->data_size,
                                 *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->back_block - i) * deque->data_size,
                                 AETHER_BLOCKS_ADD_SIZE);
            }

            deque->back_block = center;
            deque->front_block = deque->back_block - num_blocks + 1;
        }
    }

    return;
}

static inline void resize_ae_deq(ae_deq *deque)
{
    size_t old_size_blocks = deque->pointers.max_quant;

    size_t offset = old_size_blocks / 2;
    deque->front_block += offset;
    deque->back_block += offset;
    resize_ae_base(&deque->pointers, &pointer_size, deque->pointers.max_quant * 2);
    memcpy(deque->pointers.memory + offset * pointer_size, deque->pointers.memory, old_size_blocks * pointer_size);

    resize_ae_base(&deque->blocks, &deque->data_size, deque->pointers.max_quant * AETHER_BLOCKS_ADD_SIZE);

    size_t it = (deque->pointers.max_quant - old_size_blocks) / 2;
    offset = it + deque->pointers.max_quant / 2;
    for (size_t i = 0; i < it; i++)
    {
        *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i) = old_size_blocks + i;
        *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i + offset) = old_size_blocks + i + it;
    }

    return;
}

static inline void minimize_ae_deq(ae_deq *deque)
{
    if (deque->pointers.max_quant / (deque->back_block - deque->front_block + 1) > 8)
    {
        size_t num_blocks = deque->back_block - deque->front_block + 1;
        size_t center = deque->pointers.max_quant / 2;

        if (deque->front_block < deque->pointers.max_quant - deque->back_block - 1)
        {
            center -= num_blocks / 2;

            if (deque->front_block != center)
            {
                for (size_t i = 0; i < num_blocks; i++)
                {
                    set_base_ae_base(&deque->blocks, &deque->blocks, &deque->data_size, *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i + center) * deque->data_size, *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i + deque->front_block) * deque->data_size, AETHER_BLOCKS_ADD_SIZE);
                }

                deque->front_block = center;
                deque->back_block = deque->front_block + num_blocks - 1;
            }
        }
        else if (deque->front_block > deque->pointers.max_quant - deque->back_block - 1)
        {
            center += num_blocks / 2 - 1;

            if (deque->back_block != center)
            {
                for (size_t i = 0; i < num_blocks; i++)
                {
                    set_base_ae_base(&deque->blocks, &deque->blocks, &deque->data_size, *(size_t *)get_ae_base(&deque->pointers, &pointer_size, center - i) * deque->data_size, *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->back_block - i) * deque->data_size, AETHER_BLOCKS_ADD_SIZE);
                }
            }
            deque->back_block = center;
            deque->front_block = deque->back_block - num_blocks + 1;
        }

        memcpy(deque->pointers.memory, deque->pointers.memory + (deque->pointers.max_quant / 4) * pointer_size, (deque->pointers.max_quant / 2) * pointer_size);
        deque->front_block -= deque->pointers.max_quant / 4;
        deque->back_block -= deque->pointers.max_quant / 4;
        resize_ae_base(&deque->pointers, &pointer_size, deque->pointers.max_quant / 2);
        resize_ae_base(&deque->blocks, &deque->data_size, deque->pointers.max_quant * AETHER_BLOCKS_ADD_SIZE);
    }
    else
    {
        reorganize_ae_deq(deque);
    }

    return;
}

void *push_front_ae_deq(ae_deq *const deque)
{
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
                resize_ae_deq(deque);
            deque->front_block--;
        }
        deque->front_i = deque->front_i == 0 ? AETHER_BLOCKS_ADD_SIZE - 1 : deque->front_i - 1;
    }

    return deque->blocks.memory + *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->front_block) * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->front_i * deque->data_size;
}

void pop_front_ae_deq(ae_deq *const deque, void *const par)
{
    assert(deque->front_i != -1 && deque->back_i != -1);

    if (par != NULL)
        memcpy(par, deque->blocks.memory + *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->front_block) * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->front_i * deque->data_size, deque->data_size);
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
        if (deque->front_i == AETHER_BLOCKS_ADD_SIZE - 1)
        {
            deque->front_block++;
            minimize_ae_deq(deque);
        }
        deque->front_i = (deque->front_i + 1) % AETHER_BLOCKS_ADD_SIZE;
    }

    return;
}

void *front_ae_deq(const ae_deq *const deque)
{
    assert(deque->front_i != -1 && deque->back_i != -1);

    return deque->blocks.memory + *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->front_block) * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->front_i * deque->data_size;
}

void *push_back_ae_deq(ae_deq *const deque)
{
    if (deque->front_i == -1 && deque->back_i == -1)
    {
        deque->front_i = 0;
        deque->back_i = 0;
    }
    else
    {
        if (deque->back_i == AETHER_BLOCKS_ADD_SIZE - 1)
        {
            if (deque->back_block == deque->pointers.quant - 1)
                resize_ae_deq(deque);
            deque->back_block++;
        }
        deque->back_i = (deque->back_i + 1) % AETHER_BLOCKS_ADD_SIZE;
    }

    return deque->blocks.memory + *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->back_block) * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->back_i * deque->data_size;
}

void pop_back_ae_deq(ae_deq *const deque, void *const par)
{
    assert(deque->front_i != -1 && deque->back_i != -1);

    if (par != NULL)
        memcpy(par, deque->blocks.memory + *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->back_block) * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->back_i * deque->data_size, deque->data_size);
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
            minimize_ae_deq(deque);
        }
        deque->back_i = deque->back_i == 0 ? AETHER_BLOCKS_ADD_SIZE - 1 : deque->back_i - 1;
    }

    return;
}

void *back_ae_deq(const ae_deq *const deque)
{
    assert(deque->front_i != -1 && deque->back_i != -1);

    return deque->blocks.memory + *(size_t *)get_ae_base(&deque->pointers, &pointer_size, deque->back_block) * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->back_i * deque->data_size;
}

void *get_ae_deq(const ae_deq *const deque, size_t i)
{
    assert(i >= 0 && i < quant_ae_deq(deque));

    size_t link_i = (i + deque->front_i) / AETHER_BLOCKS_ADD_SIZE;
    i = (i + deque->front_i) - AETHER_BLOCKS_ADD_SIZE * link_i;
    link_i += deque->front_block;

    return deque->blocks.memory + *(size_t *)get_ae_base(&deque->pointers, &pointer_size, link_i) * deque->data_size * AETHER_BLOCKS_ADD_SIZE + i * deque->data_size;
}

void *insert_ae_deq(ae_deq *const deque, size_t i)
{
    size_t quant = quant_ae_deq(deque);
    assert(i >= 0 && i < quant);

    if (i == 0)
        return push_front_ae_deq(deque);
    else if (i == quant)
        return push_back_ae_deq(deque);

    if (deque->back_i == AETHER_BLOCKS_ADD_SIZE - 1)
    {
        if (deque->back_block == deque->pointers.quant - 1)
            resize_ae_deq(deque);
        deque->back_block++;
    }
    deque->back_i = (deque->back_i + 1) % AETHER_BLOCKS_ADD_SIZE;

    size_t link_f_i = (i + deque->front_i) / AETHER_BLOCKS_ADD_SIZE;
    size_t k = (i + deque->front_i) - AETHER_BLOCKS_ADD_SIZE * link_f_i;
    link_f_i += deque->front_block;
    size_t pointer1;
    size_t pointer2;
    for (size_t i = deque->back_block; i > link_f_i; i--)
    {
        pointer1 = *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i);
        pointer2 = *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i - 1);
        memcpy(deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE + 1) * deque->data_size, deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - 1) * deque->data_size);
        memcpy(deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + (AETHER_BLOCKS_ADD_SIZE - 1)) * deque->data_size, deque->data_size);
    }
    pointer2 = *(size_t *)get_ae_base(&deque->pointers, &pointer_size, link_f_i);
    memcpy(deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k + 1) * deque->data_size, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - k - 1) * deque->data_size);

    return deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size;
}

void delete_ae_deq(ae_deq *const deque, size_t i, void *const par)
{
    size_t quant = quant_ae_deq(deque);
    assert(i >= 0 && i < quant);

    if (i == 0)
    {
        pop_front_ae_deq(deque, par);
        return;
    }
    else if (i == quant - 1)
    {
        pop_back_ae_deq(deque, par);
        return;
    }

    size_t link_f_i = (i + deque->front_i) / AETHER_BLOCKS_ADD_SIZE;
    size_t k = (i + deque->front_i) - AETHER_BLOCKS_ADD_SIZE * link_f_i;
    link_f_i += deque->front_block;
    size_t pointer1;
    size_t pointer2;
    for (size_t i = deque->back_block; i > link_f_i; i--)
    {
        pointer1 = *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i);
        pointer2 = *(size_t *)get_ae_base(&deque->pointers, &pointer_size, i - 1);
        memcpy(deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + (AETHER_BLOCKS_ADD_SIZE - 1)) * deque->data_size, deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, deque->data_size);
        memcpy(deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE + 1) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - 1) * deque->data_size);
    }
    pointer2 = *(size_t *)get_ae_base(&deque->pointers, &pointer_size, link_f_i);
    if (par != NULL)
        memcpy(par, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size, deque->data_size);
    memcpy(deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k + 1) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - k - 1) * deque->data_size);

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
            minimize_ae_deq(deque);
        }
        deque->back_i = deque->back_i == 0 ? AETHER_BLOCKS_ADD_SIZE - 1 : deque->back_i - 1;
    }

    return;
}
