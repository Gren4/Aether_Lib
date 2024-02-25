#include "aether_deque.h"
#include <string.h>

#define AETHER_BLOCKS_ADD_SIZE 8
#define AETHER_DEQUE_ADD_SIZE 2

const size_t pointer_size = sizeof(size_t);

ae_deque create_ae_deque(size_t data_size)
{
    ae_deque new_deque = {
        .pointers = init_ae_base(),
        .blocks = init_ae_base(),
        .data_size = data_size,
        .front_i = -1,
        .back_i = -1,
        .front_block = AETHER_DEQUE_ADD_SIZE / 2,
        .back_block = AETHER_DEQUE_ADD_SIZE / 2};

    prepare_ae_deque(&new_deque);

    return new_deque;
}

void prepare_ae_deque(ae_deque *deque)
{
    if (create_ae_base(&deque->pointers, &pointer_size, AETHER_DEQUE_ADD_SIZE) != 0)
        return;
    if (create_max_size_ae_base(&deque->blocks, &deque->data_size, AETHER_DEQUE_ADD_SIZE * AETHER_BLOCKS_ADD_SIZE) != 0)
        return;
    for (size_t i = 0; i < AETHER_DEQUE_ADD_SIZE; i++)
    {
        if (append_ae_base(&deque->pointers, &pointer_size, &i) != 0)
            return;
    }
    return;
}

uint8_t free_ae_deque(ae_deque *deque)
{
    if (deque->pointers.memory != NULL)
        free_ae_base(&deque->pointers);
    if (deque->blocks.memory != NULL)
        free_ae_base(&deque->blocks);

    return 0;
}

uint8_t reorganize_ae_deque(ae_deque *deque)
{
    if (deque->pointers.max_quant >= 16)
    {
        if (deque->front_block >= (deque->pointers.max_quant * 3) / 4)
        {
            size_t num_blocks = deque->back_block - deque->front_block + 1;

            size_t center = deque->pointers.max_quant / 2 - num_blocks / 2;
            size_t from = 0;
            size_t to = 0;

            for (size_t i = 0; i < num_blocks; i++)
            {
                get_ae_base(&deque->pointers, &pointer_size, i + deque->front_block, &from);
                get_ae_base(&deque->pointers, &pointer_size, i + center, &to);
                set_base_ae_base(&deque->blocks, &deque->blocks, &deque->data_size, to * deque->data_size, from * deque->data_size, AETHER_BLOCKS_ADD_SIZE);
            }

            deque->front_block = center;
            deque->back_block = deque->front_block + num_blocks - 1;
        }
        else if (deque->back_block <= (deque->pointers.max_quant) / 4)
        {
            size_t num_blocks = deque->back_block - deque->front_block + 1;

            size_t center = deque->pointers.max_quant / 2 + num_blocks / 2 - 1;
            size_t from = 0;
            size_t to = 0;
            for (size_t i = 0; i < num_blocks; i++)
            {
                get_ae_base(&deque->pointers, &pointer_size, deque->back_block - i, &from);
                get_ae_base(&deque->pointers, &pointer_size, center - i, &to);
                set_base_ae_base(&deque->blocks, &deque->blocks, &deque->data_size, to * deque->data_size, from * deque->data_size, AETHER_BLOCKS_ADD_SIZE);
            }

            deque->back_block = center;
            deque->front_block = deque->back_block - num_blocks + 1;
        }
    }

    return 0;
}

uint8_t resize_ae_deque(ae_deque *deque)
{
    size_t old_size_blocks = deque->pointers.max_quant;
    {
        size_t offset = old_size_blocks / 2;
        deque->front_block += offset;
        deque->back_block += offset;
        resize_ae_base(&deque->pointers, &pointer_size, deque->pointers.max_quant * 2);
        memmove(deque->pointers.memory + offset * pointer_size, deque->pointers.memory, old_size_blocks * pointer_size);
    }
    resize_ae_base(&deque->blocks, &deque->data_size, deque->pointers.max_quant * AETHER_BLOCKS_ADD_SIZE);
    size_t it = (deque->pointers.max_quant - old_size_blocks) / 2;
    size_t offset = it + deque->pointers.max_quant / 2;
    size_t j = 0;
    for (size_t i = 0; i < it; i++)
    {
        j = old_size_blocks + i;
        if (set_ae_base(&deque->pointers, &pointer_size, i, &j) != 0)
            return 1;
        j = old_size_blocks + i + it;
        if (set_ae_base(&deque->pointers, &pointer_size, i + offset, &j) != 0)
            return 2;
    }

    return 0;
}

uint8_t minimize_ae_deque(ae_deque *deque)
{
    if (deque->pointers.max_quant / (deque->back_block - deque->front_block + 1) > 8)
    {
        size_t num_blocks = deque->back_block - deque->front_block + 1;
        size_t center = deque->pointers.max_quant / 2;

        if (deque->front_block < deque->pointers.max_quant - deque->back_block - 1)
        {
            center -= num_blocks / 2;
            size_t from = 0;
            size_t to = 0;
            if (deque->front_block != center)
            {
                for (size_t i = 0; i < num_blocks; i++)
                {
                    get_ae_base(&deque->pointers, &pointer_size, i + deque->front_block, &from);
                    get_ae_base(&deque->pointers, &pointer_size, i + center, &to);
                    set_base_ae_base(&deque->blocks, &deque->blocks, &deque->data_size, to * deque->data_size, from * deque->data_size, AETHER_BLOCKS_ADD_SIZE);
                }

                deque->front_block = center;
                deque->back_block = deque->front_block + num_blocks - 1;
            }
        }
        else if (deque->front_block > deque->pointers.max_quant - deque->back_block - 1)
        {
            center += num_blocks / 2 - 1;
            size_t from = 0;
            size_t to = 0;
            if (deque->back_block != center)
            {
                for (size_t i = 0; i < num_blocks; i++)
                {
                    get_ae_base(&deque->pointers, &pointer_size, deque->back_block - i, &from);
                    get_ae_base(&deque->pointers, &pointer_size, center - i, &to);
                    set_base_ae_base(&deque->blocks, &deque->blocks, &deque->data_size, to * deque->data_size, from * deque->data_size, AETHER_BLOCKS_ADD_SIZE);
                }
            }
            deque->back_block = center;
            deque->front_block = deque->back_block - num_blocks + 1;
        }

        memmove(deque->pointers.memory, deque->pointers.memory + (deque->pointers.max_quant / 4) * pointer_size, (deque->pointers.max_quant / 2) * pointer_size);
        deque->front_block -= deque->pointers.max_quant / 4;
        deque->back_block -= deque->pointers.max_quant / 4;
        resize_ae_base(&deque->pointers, &pointer_size, deque->pointers.max_quant / 2);
        resize_ae_base(&deque->blocks, &deque->data_size, deque->pointers.max_quant * AETHER_BLOCKS_ADD_SIZE);
    }
    else
    {
        reorganize_ae_deque(deque);
    }

    return 0;
}

uint8_t push_front_ae_deque(ae_deque *deque, void *par)
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
                if (resize_ae_deque(deque) != 0)
                    return 1;
            deque->front_block--;
        }
        deque->front_i = deque->front_i == 0 ? AETHER_BLOCKS_ADD_SIZE - 1 : deque->front_i - 1;
    }
    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, deque->front_block, &pointer) != 0)
        return 4;
    memmove(deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->front_i * deque->data_size, par, deque->data_size);

    return 0;
}

uint8_t pop_front_ae_deque(ae_deque *deque, void *par)
{
    if (deque->front_i == -1 && deque->back_i == -1)
        return 1;
    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, deque->front_block, &pointer) != 0)
        return 2;
    if (par != NULL)
        memmove(par, deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->front_i * deque->data_size, deque->data_size);
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
            minimize_ae_deque(deque);
        }
        deque->front_i = (deque->front_i + 1) % AETHER_BLOCKS_ADD_SIZE;
    }

    return 0;
}

uint8_t front_ae_deque(ae_deque *deque, void *par)
{
    if (deque->front_i == -1 && deque->back_i == -1)
        return 1;
    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, deque->front_block, &pointer) != 0)
        return 2;
    if (par == NULL)
        return 3;
    memmove(par, deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->front_i * deque->data_size, deque->data_size);

    return 0;
}

uint8_t push_back_ae_deque(ae_deque *deque, void *par)
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
            {
                if (resize_ae_deque(deque) != 0)
                    return 1;
            }
            deque->back_block++;
        }
        deque->back_i = (deque->back_i + 1) % AETHER_BLOCKS_ADD_SIZE;
    }
    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, deque->back_block, &pointer) != 0)
        return 4;
    memmove(deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->back_i * deque->data_size, par, deque->data_size);

    return 0;
}

uint8_t pop_back_ae_deque(ae_deque *deque, void *par)
{
    if (deque->front_i == -1 && deque->back_i == -1)
        return 1;
    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, deque->back_block, &pointer) != 0)
        return 2;
    if (par != NULL)
        memmove(par, deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->back_i * deque->data_size, deque->data_size);
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
            minimize_ae_deque(deque);
        }
        deque->back_i = deque->back_i == 0 ? AETHER_BLOCKS_ADD_SIZE - 1 : deque->back_i - 1;
    }

    return 0;
}

uint8_t back_ae_deque(ae_deque *deque, void *par)
{
    if (deque->front_i == -1 && deque->back_i == -1)
        return 1;
    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, deque->back_block, &pointer) != 0)
        return 2;
    if (par == NULL)
        return 3;
    memmove(par, deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + deque->back_i * deque->data_size, deque->data_size);

    return 0;
}

uint8_t get_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (i < 0 || i > (((deque->back_block - deque->front_block) - 1) * AETHER_BLOCKS_ADD_SIZE) + (deque->back_i + (AETHER_BLOCKS_ADD_SIZE - deque->front_i)))
        return 4;

    size_t link_i = (i + deque->front_i) / AETHER_BLOCKS_ADD_SIZE;
    i = (i + deque->front_i) - AETHER_BLOCKS_ADD_SIZE * link_i;
    link_i += deque->front_block;

    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, link_i, &pointer) != 0)
        return 5;

    memmove(par, deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + i * deque->data_size, deque->data_size);
    return 0;
}

uint8_t set_ae_deque(ae_deque *deque, size_t i, void *par)
{
    if (i < 0 || i > (((deque->back_block - deque->front_block) - 1) * AETHER_BLOCKS_ADD_SIZE) + (deque->back_i + (AETHER_BLOCKS_ADD_SIZE - deque->front_i)))
        return 1;

    size_t link_i = (i + deque->front_i) / AETHER_BLOCKS_ADD_SIZE;
    i = (i + deque->front_i) - AETHER_BLOCKS_ADD_SIZE * link_i;
    link_i += deque->front_block;

    size_t pointer;
    if (get_ae_base(&deque->pointers, &pointer_size, link_i, &pointer) != 0)
        return 5;

    memmove(deque->blocks.memory + pointer * deque->data_size * AETHER_BLOCKS_ADD_SIZE + i * deque->data_size, par, deque->data_size);
    return 0;
}

uint8_t insert_ae_deque(ae_deque *deque, size_t i, void *par)
{
    size_t quant = (((deque->back_block - deque->front_block) - 1) * AETHER_BLOCKS_ADD_SIZE) + (deque->back_i + 1 + (AETHER_BLOCKS_ADD_SIZE - deque->front_i));
    if (i < 0 || i > quant)
        return 1;

    if (i == 0)
    {
        if (push_front_ae_deque(deque, par) != 0)
            return 2;
        return 0;
    }
    else if (i == quant)
    {
        if (push_back_ae_deque(deque, par) != 0)
            return 3;
        return 0;
    }

    if (deque->back_i == AETHER_BLOCKS_ADD_SIZE - 1)
    {
        if (deque->back_block == deque->pointers.quant - 1)
        {
            if (resize_ae_deque(deque) != 0)
                return 1;
        }
        deque->back_block++;
    }
    deque->back_i = (deque->back_i + 1) % AETHER_BLOCKS_ADD_SIZE;

    size_t link_f_i = (i + deque->front_i) / AETHER_BLOCKS_ADD_SIZE;
    size_t k = (i + deque->front_i) - AETHER_BLOCKS_ADD_SIZE * link_f_i;
    link_f_i += deque->front_block;
    size_t pointer1 = 0;
    size_t pointer2 = 0;
    for (size_t i = deque->back_block; i > link_f_i; i--)
    {
        get_ae_base(&deque->pointers, &pointer_size, i, &pointer1);
        get_ae_base(&deque->pointers, &pointer_size, i - 1, &pointer2);
        memmove(deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE + 1) * deque->data_size, deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - 1) * deque->data_size);
        memmove(deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + (AETHER_BLOCKS_ADD_SIZE - 1)) * deque->data_size, deque->data_size);
    }
    get_ae_base(&deque->pointers, &pointer_size, link_f_i, &pointer2);
    memmove(deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k + 1) * deque->data_size, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - k - 1) * deque->data_size);
    memmove(deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size, par, deque->data_size);

    return 0;
}

uint8_t delete_ae_deque(ae_deque *deque, size_t i, void *par)
{
    size_t quant = (((deque->back_block - deque->front_block) - 1) * AETHER_BLOCKS_ADD_SIZE) + (deque->back_i + 1 + (AETHER_BLOCKS_ADD_SIZE - deque->front_i));
    if (i < 0 || i > quant)
        return 1;

    if (i == 0)
    {
        if (pop_front_ae_deque(deque, par) != 0)
            return 2;
        return 0;
    }
    else if (i == quant - 1)
    {
        if (pop_back_ae_deque(deque, par) != 0)
            return 3;
        return 0;
    }

    size_t link_f_i = (i + deque->front_i) / AETHER_BLOCKS_ADD_SIZE;
    size_t k = (i + deque->front_i) - AETHER_BLOCKS_ADD_SIZE * link_f_i;
    link_f_i += deque->front_block;
    size_t pointer1 = 0;
    size_t pointer2 = 0;
    for (size_t i = deque->back_block; i > link_f_i; i--)
    {
        get_ae_base(&deque->pointers, &pointer_size, i, &pointer1);
        get_ae_base(&deque->pointers, &pointer_size, i - 1, &pointer2);
        memmove(deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + (AETHER_BLOCKS_ADD_SIZE - 1)) * deque->data_size, deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, deque->data_size);
        memmove(deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE) * deque->data_size, deque->blocks.memory + (pointer1 * AETHER_BLOCKS_ADD_SIZE + 1) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - 1) * deque->data_size);
    }
    get_ae_base(&deque->pointers, &pointer_size, link_f_i, &pointer2);
    if (par != NULL)
        memmove(par, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size, deque->data_size);
    memmove(deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k) * deque->data_size, deque->blocks.memory + (pointer2 * AETHER_BLOCKS_ADD_SIZE + k + 1) * deque->data_size, (AETHER_BLOCKS_ADD_SIZE - k - 1) * deque->data_size);

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
            minimize_ae_deque(deque);
        }
        deque->back_i = deque->back_i == 0 ? AETHER_BLOCKS_ADD_SIZE - 1 : deque->back_i - 1;
    }

    return 0;
}
