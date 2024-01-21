#include "aether_base.h"

ae_base init_ae_base(void)
{
    ae_base new_base = {
        .memory = NULL,
        .quant = 0};

    return new_base;
}

uint8_t create_ae_base(ae_base *base, size_t data_size, size_t storage_size)
{

    if (base == NULL)
        return 1;

    base->memory = calloc(storage_size, data_size);

    if (base->memory == NULL)
        return 2;

#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", base->memory);
#endif

    base->quant = 0;

    return 0;
}

uint8_t free_ae_base(ae_base *base)
{
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", base->memory);
#endif
    free(base->memory);
    base->quant = 0;

    return 0;
}

uint8_t resize_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t new_size)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (new_size == base->quant)
        return 0;

    void *p = realloc(base->memory, (((new_size / storage_size) + 1) * storage_size) * data_size);

    if (p == NULL)
        return 3;

#ifdef DEBUG_AE
    printf("Reallocated memory from address %p to %p\n", base->memory, p);
#endif
    base->memory = p;

    if (new_size > base->quant)
        memset(base->memory + base->quant * data_size, 0, (new_size - base->quant) * data_size);

    base->quant = new_size;

    return 0;
}

uint8_t append_ae_base(ae_base *base, size_t data_size, size_t storage_size, void *par)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (par == NULL)
        return 3;

    if ((base->quant + 1) % storage_size == 0)
    {
        void *p = realloc(base->memory, ((base->quant + 1) + storage_size) * data_size);

        if (p == NULL)
            return 4;

#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", base->memory, p);
#endif
        base->memory = p;
    }

    memmove(base->memory + base->quant * data_size, par, data_size);

    base->quant++;

    return 0;
}

uint8_t append_base_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size, size_t i, size_t n)
{

    if (base_to == NULL)
        return 1;
    if (base_from == NULL)
        return 2;
    if (base_to->memory == NULL)
        return 3;
    if (base_from->memory == NULL)
        return 4;
    if (i < 0 || i >= base_from->quant)
        return 5;

    if ((base_from->quant - i) < n)
        n = base_from->quant - i;

    if ((((base_to->quant % storage_size) + n) / storage_size) >= 1)
    {
        void *p = realloc(base_to->memory, ((((base_to->quant + n) / storage_size) + 1) * storage_size) * data_size);

        if (p == NULL)
            return 6;

#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", base_to->memory, p);
#endif
        base_to->memory = p;
    }

    memmove(base_to->memory + base_to->quant * data_size, base_from->memory + i * data_size, n * data_size);

    base_to->quant += n;

    return 0;
}

uint8_t set_ae_base(ae_base *base, size_t data_size, size_t i, void *par)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (i < 0 || i >= base->quant)
        return 3;
    if (par == NULL)
        return 4;

    memmove(base->memory + i * data_size, par, data_size);

    return 0;
}

uint8_t get_ae_base(ae_base *base, size_t data_size, size_t i, void *par)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (i < 0 || i >= base->quant)
        return 3;
    if (par == NULL)
        return 4;

    memmove(par, base->memory + i * data_size, data_size);

    return 0;
}

uint8_t get_pointer_ae_base(ae_base *base, size_t data_size, size_t i, void **par)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (i < 0 || i >= base->quant)
        return 3;
    if (par == NULL)
        return 4;

    *par = base->memory + i * data_size;

    return 0;
}

uint8_t concat_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size)
{

    if (base_to == NULL)
        return 1;
    if (base_from == NULL)
        return 2;
    if (base_to->memory == NULL)
        return 3;
    if (base_from->memory == NULL)
        return 4;

    size_t new_size = base_to->quant + base_from->quant;
    void *p = realloc(base_to->memory, (((new_size / storage_size) + 1) * storage_size) * data_size);

    if (p == NULL)
        return 5;

#ifdef DEBUG_AE
    printf("Reallocated memory from address %p to %p\n", base_to->memory, p);
#endif

    base_to->memory = p;

    memmove(base_to->memory + base_to->quant * data_size, base_from->memory, base_from->quant * data_size);

    base_to->quant = new_size;

    return 0;
}

uint8_t insert_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (par == NULL)
        return 3;

    if ((base->quant + 1) % storage_size == 0)
    {
        void *p = realloc(base->memory, ((base->quant + 1) + storage_size) * data_size);

        if (p == NULL)
            return 4;

#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", base->memory, p);
#endif

        base->memory = p;
    }

    memmove(base->memory + (i + 1) * data_size, base->memory + i * data_size, (base->quant - i) * data_size);
    memmove(base->memory + i * data_size, par, data_size);

    base->quant++;

    return 0;
}

uint8_t insert_base_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size, size_t i_to, size_t i_from, size_t n)
{

    if (base_to == NULL)
        return 1;
    if (base_from == NULL)
        return 2;
    if (base_to->memory == NULL)
        return 3;
    if (base_from->memory == NULL)
        return 4;
    if (i_to < 0 || i_to >= base_to->quant)
        return 5;
    if (i_from < 0 || i_from >= base_from->quant)
        return 6;

    if ((base_from->quant - i_from) < n)
        n = base_from->quant - i_from;

    if ((((base_to->quant % storage_size) + n) / storage_size) >= 1)
    {
        void *p = realloc(base_to->memory, ((((base_to->quant + n) / storage_size) + 1) * storage_size) * data_size);

        if (p == NULL)
            return 7;

#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", base_to->memory, p);
#endif

        base_to->memory = p;
    }
    memmove(base_to->memory + (i_to + n) * data_size, base_to->memory + i_to * data_size, (base_to->quant - i_to) * data_size);
    memmove(base_to->memory + i_to * data_size, base_from->memory + i_from * data_size, n * data_size);

    base_to->quant += n;

    return 0;
}

uint8_t delete_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (i < 0 || i >= base->quant)
        return 3;

    if ((base->quant - 1) % (2 * storage_size) == 0)
    {
        void *p = realloc(base->memory, (((base->quant - 1) / storage_size) + 1) * storage_size * data_size);

        if (p == NULL)
            return 4;

#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", base->memory, p);
#endif

        base->memory = p;
    }

    if (par != NULL)
        memmove(par, base->memory + i * data_size, data_size);
    if (i != base->quant - 1)
        memmove(base->memory + i * data_size, base->memory + (i + 1) * data_size, (base->quant - i) * data_size);

    base->quant--;

    return 0;
}

uint8_t duplicate_ae_base(ae_base *base_out, ae_base *base_in, size_t data_size, size_t storage_size)
{

    if (base_out == NULL)
        return 1;

    if (base_out->memory != NULL)
        free_ae_base(base_out);

    if (create_ae_base(base_out, data_size, storage_size) != 0)
        return 2;
    if (resize_ae_base(base_out, data_size, storage_size, base_in->quant) != 0)
        return 3;

    if (base_in->memory != NULL)
        memmove(base_out->memory, base_in->memory, base_in->quant * data_size);

    return 0;
}

uint8_t invert_ae_base(ae_base *base, size_t data_size)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;

    if (base->quant > 1)
    {
        size_t iterations = base->quant / 2;
        size_t i = 0;
        void *par = malloc(data_size);

        if (par == NULL)
            return 3;

        for (i = 0; i < iterations; i++)
        {
            memmove(par, base->memory + i * data_size, data_size);
            memmove(base->memory + i * data_size, base->memory + (base->quant - 1 - i) * data_size, data_size);
            memmove(base->memory + (base->quant - 1 - i) * data_size, par, data_size);
        }

        free(par);
    }

    return 0;
}

uint8_t find_ae_base(ae_base *base, size_t data_size, void *par, size_t *ret)
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;
    if (par == NULL)
        return 3;

    *ret = base->quant;

    size_t i = 0;
    for (i = 0; i < base->quant; i++)
    {
        if (memcmp(par, base->memory + i * data_size, data_size) == 0)
        {
            *ret = i;
            return 0;
        }
    }

    return 0;
}

uint8_t sort_ae_base(ae_base *base, size_t data_size, int (*comparator)(const void *, const void *))
{

    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;

    qsort(base->memory, base->quant, data_size, comparator);

    return 0;
}

uint8_t swap_ae_base(ae_base *base, size_t data_size, size_t i, size_t j)
{
    if (base == NULL)
        return 1;
    if (base->memory == NULL)
        return 2;

    void *p = malloc(data_size);
    if (p == NULL)
        return 3;

    memmove(p, base->memory + j * data_size, data_size);
    memmove(base->memory + j * data_size, base->memory + i * data_size, data_size);
    memmove(base->memory + i * data_size, p, data_size);

    free(p);

    return 0;
}
