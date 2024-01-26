#include "aether_base.h"
#include <math.h>

void calc_power_ae_base(AE_BASE_MEM type, uint8_t *power, size_t size)
{
    switch (type)
    {
    case AE_BASE_RECOUNT:
        *power = 0;
    case AE_BASE_INCR:
        while (AETHER_BASE_ADD_SIZE * pow(2, *power) <= size)
        {
            *power += 1;
        }
        break;

    case AE_BASE_DECR:
        while (AETHER_BASE_ADD_SIZE * pow(2, *power) > size && *power != 0)
        {
            *power -= 1;
        }
        *power += 1;
        break;

    default:
        break;
    }

    return;
}

uint8_t check_realloc_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size, AE_BASE_MEM type)
{
    size_t cur_size = AETHER_BASE_ADD_SIZE * (int)(pow(2, base->power) + 0.5);
    uint8_t check = 0;

    switch (type)
    {
    case AE_BASE_RECOUNT:
    case AE_BASE_INCR:
        check = new_size >= cur_size;
        break;

    case AE_BASE_DECR:
        check = new_size <= cur_size / 8;
        break;

    default:
        break;
    }

    if (check)
    {
        calc_power_ae_base(type, &base->power, new_size);
        void *p = realloc(base->memory, AETHER_BASE_ADD_SIZE * (int)(pow(2, base->power) + 0.5) * (*data_size));

        if (p == NULL)
            return 1;

#ifdef DEBUG_AE
        printf("Reallocated memory from address %p to %p\n", base->memory, p);
#endif

        base->memory = p;
    }

    return 0;
}

ae_base init_ae_base(void)
{
    ae_base new_base = {
        .memory = NULL,
        .quant = 0,
        .power = 0};

    return new_base;
}

uint8_t create_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size)
{
    calc_power_ae_base(AE_BASE_INCR, &base->power, new_size);
    base->memory = calloc(AETHER_BASE_ADD_SIZE * (int)(pow(2, base->power) + 0.5), *data_size);

    if (base->memory == NULL)
        return 1;

#ifdef DEBUG_AE
    printf("Allocated memory at address %p\n", base->memory);
#endif

    base->quant = 0;

    return 0;
}

uint8_t free_ae_base(ae_base *const base)
{
#ifdef DEBUG_AE
    printf("Freed memory at address %p\n", base->memory);
#endif
    free(base->memory);
    base->quant = 0;

    return 0;
}

uint8_t resize_ae_base(ae_base *const base, const size_t *const data_size, size_t new_size)
{
    if (new_size == base->quant)
        return 0;

    if (check_realloc_ae_base(base, data_size, new_size, AE_BASE_RECOUNT) != 0)
        return 1;

    if (new_size > base->quant)
        memset(base->memory + base->quant * (*data_size), 0, (new_size - base->quant) * (*data_size));

    base->quant = new_size;

    return 0;
}

uint8_t append_ae_base(ae_base *const base, const size_t *const data_size, void *par)
{
    size_t new_size = base->quant + 1;
    if (check_realloc_ae_base(base, data_size, new_size, AE_BASE_INCR) != 0)
        return 4;

    memmove(base->memory + base->quant * (*data_size), par, *data_size);

    base->quant++;

    return 0;
}

uint8_t append_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i, size_t n)
{
    if (i < 0 || i >= base_from->quant)
        return 1;

    if ((base_from->quant - i) < n)
        n = base_from->quant - i;

    size_t new_size = base_to->quant + n;
    if (check_realloc_ae_base(base_to, data_size, new_size, AE_BASE_INCR) != 0)
        return 2;

    memmove(base_to->memory + base_to->quant * (*data_size), base_from->memory + i * (*data_size), n * (*data_size));

    base_to->quant += n;

    return 0;
}

uint8_t set_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *par)
{
    if (i < 0 || i >= base->quant)
        return 1;

    memmove(base->memory + i * (*data_size), par, *data_size);

    return 0;
}

uint8_t get_ae_base(const ae_base *const base, const size_t *const data_size, size_t i, void *par)
{
    if (i < 0 || i >= base->quant)
        return 1;

    memmove(par, base->memory + i * (*data_size), *data_size);

    return 0;
}

uint8_t get_pointer_ae_base(const ae_base *const base, const size_t *const data_size, size_t i, void **par)
{
    if (i < 0 || i >= base->quant)
        return 1;

    *par = base->memory + i * (*data_size);

    return 0;
}

uint8_t concat_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size)
{
    size_t new_size = base_to->quant + base_from->quant;
    if (check_realloc_ae_base(base_to, data_size, new_size, AE_BASE_INCR) != 0)
        return 1;
    memmove(base_to->memory + base_to->quant * (*data_size), base_from->memory, base_from->quant * (*data_size));

    base_to->quant = new_size;

    return 0;
}

uint8_t insert_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *par)
{
    size_t new_size = base->quant + 1;
    if (check_realloc_ae_base(base, data_size, new_size, AE_BASE_INCR) != 0)
        return 1;

    memmove(base->memory + (i + 1) * (*data_size), base->memory + i * (*data_size), (base->quant - i) * (*data_size));
    memmove(base->memory + i * (*data_size), par, *data_size);

    base->quant++;

    return 0;
}

uint8_t insert_base_ae_base(ae_base *const base_to, ae_base *const base_from, const size_t *const data_size, size_t i_to, size_t i_from, size_t n)
{
    if (i_to < 0 || i_to >= base_to->quant)
        return 1;
    if (i_from < 0 || i_from >= base_from->quant)
        return 2;

    if ((base_from->quant - i_from) < n)
        n = base_from->quant - i_from;

    size_t new_size = base_to->quant + n;
    if (check_realloc_ae_base(base_to, data_size, new_size, AE_BASE_INCR) != 0)
        return 3;

    memmove(base_to->memory + (i_to + n) * (*data_size), base_to->memory + i_to * (*data_size), (base_to->quant - i_to) * (*data_size));
    memmove(base_to->memory + i_to * (*data_size), base_from->memory + i_from * (*data_size), n * (*data_size));

    base_to->quant += n;

    return 0;
}

uint8_t delete_ae_base(ae_base *const base, const size_t *const data_size, size_t i, void *par)
{
    if (i < 0 || i >= base->quant)
        return 1;

    if (par != NULL)
        memmove(par, base->memory + i * (*data_size), *data_size);
    if (i != base->quant - 1)
        memmove(base->memory + i * (*data_size), base->memory + (i + 1) * (*data_size), (base->quant - i) * (*data_size));

    size_t new_size = base->quant - 1;
    if (check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR) != 0)
        return 2;

    base->quant--;

    return 0;
}

uint8_t delete_base_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t n, ae_base *const par)
{
    if (i < 0 || i >= base->quant)
        return 1;
    if (n > base->quant - i)
        n = base->quant - i;

    if (par != NULL)
    {
        check_realloc_ae_base(par, data_size, n, AE_BASE_DECR);
        memmove(par->memory, base->memory + i * (*data_size), n * (*data_size));
        par->quant = n;
    }
    if (i != base->quant - n)
        memmove(base->memory + i * (*data_size), base->memory + (i + n) * (*data_size), (base->quant - i - n) * (*data_size));

    size_t new_size = base->quant - n;
    if (check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR) != 0)
        return 2;

    base->quant -= n;

    return 0;
}

uint8_t pop_ae_base(ae_base *const base, const size_t *const data_size, void *par)
{
    if (base->quant == 0)
        return 1;

    if (par != NULL)
        memmove(par, base->memory + (base->quant - 1) * (*data_size), *data_size);

    size_t new_size = base->quant - 1;
    if (check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR) != 0)
        return 2;

    base->quant--;

    return 0;
}

uint8_t pop_base_ae_base(ae_base *const base, const size_t *const data_size, size_t n, ae_base *const par)
{
    if (base->quant == 0)
        return 1;
    if (n > base->quant)
        n = base->quant;

    if (par != NULL)
    {
        check_realloc_ae_base(par, data_size, n, AE_BASE_RECOUNT);
        memmove(par->memory, base->memory + (base->quant - n) * (*data_size), n * (*data_size));
        par->quant = n;
    }

    size_t new_size = base->quant - n;
    if (check_realloc_ae_base(base, data_size, new_size, AE_BASE_DECR) != 0)
        return 2;

    base->quant -= n;

    return 0;
}

uint8_t duplicate_ae_base(ae_base *const base_out, const ae_base *const base_in, const size_t *const data_size)
{
    if (base_out->memory != NULL)
        free_ae_base(base_out);

    if (create_ae_base(base_out, data_size, base_in->quant) != 0)
        return 1;

    if (base_in->memory != NULL)
        memmove(base_out->memory, base_in->memory, base_in->quant * (*data_size));

    return 0;
}

uint8_t invert_ae_base(ae_base *const base, const size_t *const data_size)
{
    if (base->quant > 1)
    {
        size_t iterations = base->quant / 2;
        size_t i = 0;
        void *par = malloc(*data_size);

        if (par == NULL)
            return 3;

        for (i = 0; i < iterations; i++)
        {
            memmove(par, base->memory + i * (*data_size), *data_size);
            memmove(base->memory + i * (*data_size), base->memory + (base->quant - 1 - i) * (*data_size), *data_size);
            memmove(base->memory + (base->quant - 1 - i) * (*data_size), par, *data_size);
        }

        free(par);
    }

    return 0;
}

uint8_t find_ae_base(const ae_base *const base, const size_t *const data_size, void *par, size_t *ret)
{
    *ret = base->quant;

    size_t i = 0;
    for (i = 0; i < base->quant; i++)
    {
        if (memcmp(par, base->memory + i * (*data_size), *data_size) == 0)
        {
            *ret = i;
            return 0;
        }
    }

    return 0;
}

uint8_t sort_ae_base(ae_base *const base, const size_t *const data_size, int (*comparator)(const void *, const void *))
{
    qsort(base->memory, base->quant, *data_size, comparator);

    return 0;
}

uint8_t swap_ae_base(ae_base *const base, const size_t *const data_size, size_t i, size_t j)
{
    void *p = malloc(*data_size);
    if (p == NULL)
        return 1;

    memmove(p, base->memory + j * (*data_size), *data_size);
    memmove(base->memory + j * (*data_size), base->memory + i * (*data_size), *data_size);
    memmove(base->memory + i * (*data_size), p, *data_size);

    free(p);

    return 0;
}
