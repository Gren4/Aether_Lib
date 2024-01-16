#include "aether_base.h"

ae_base f_init_ae_base(void)
{
    ae_base new_base = {
        .memory = NULL,
        .quant = 0
    };

    return new_base;
}

uint8_t f_create_ae_base(ae_base *base, size_t data_size, size_t storage_size)
{
    if (base == NULL)
        return 1;

    base->memory = calloc(storage_size, data_size);
#ifdef DEBUG_AE
    printf("Allocated %x\n", base->memory);
#endif
    if (base->memory == NULL)
        return 2;

    base->quant = 0;

    return 0;
}

uint8_t f_free_ae_base(ae_base *base)
{
    #ifdef DEBUG_AE
        printf("Freed %x\n", base->memory);
    #endif
    free(base->memory);
    base->quant = 0;

    return 0;
}

uint8_t f_resize_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t new_size)
{
    if (base == NULL)
        return 1;
    if (new_size == base->quant)
        return 0;

    void *p = realloc(base->memory, (((new_size / storage_size) + 1) * storage_size) * data_size);
    #ifdef DEBUG_AE
        printf("Allocated %x\n", p);
    #endif
    if (p == NULL)
        return 2;

    base->memory = p;

    if (new_size > base->quant)
        memset(base->memory + base->quant * data_size, 0, (new_size - base->quant) * data_size);

    base->quant = new_size;

    return 0;
}

uint8_t f_append_ae_base(ae_base *base, size_t data_size, size_t storage_size, void *par)
{
    if (base->memory == NULL)
        return 1;
    if (par == NULL)
        return 2;

    if ((base->quant + 1) % storage_size == 0)
    {
        void *p = realloc(base->memory, ((base->quant + 1) + storage_size) * data_size);
        #ifdef DEBUG_AE
            printf("Allocated %x\n", p);
        #endif
        if (p == NULL)
            return 3;

        base->memory = p;
    }

    memmove(base->memory + base->quant * data_size, par, data_size);

    base->quant++;

    return 0;
}

uint8_t f_set_ae_base(ae_base *base, size_t data_size, size_t i, void *par)
{
    if (base->memory == NULL)
        return 1;
    if (i < 0 || i >= base->quant)
        return 2;
    if (par == NULL)
        return 3;

    memmove(base->memory + i * data_size, par, data_size);

    return 0;
}

uint8_t f_get_ae_base(ae_base *base, size_t data_size, size_t i, void *par)
{
    if (base->memory == NULL)
        return 1;
    if (i < 0 || i >= base->quant)
        return 2;
    if (par == NULL)
        return 3;

    memmove(par, base->memory + i * data_size, data_size);

    return 0;
}

uint8_t f_concat_ae_base(ae_base *base_to, ae_base *base_from, size_t data_size, size_t storage_size)
{
    if (base_to->memory == NULL)
        return 1;
    if (base_from->memory == NULL)
        return 2;

    size_t new_size = base_to->quant + base_from->quant;
    void *p = realloc(base_to->memory, (((new_size / storage_size) + 1) * storage_size) * data_size);
    #ifdef DEBUG_AE
        printf("Allocated %x\n", p);
    #endif
    if (p == NULL)
        return 3;

    base_to->memory = p;

    memmove(base_to->memory + base_to->quant * data_size, base_from->memory, base_from->quant * data_size);

    base_to->quant = new_size;

    return 0;
}

uint8_t f_insert_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par)
{
    if (base->memory == NULL)
        return 1;
    if (par == NULL)
        return 2;

    if ((base->quant + 1) % storage_size == 0)
    {
        void *p = realloc(base->memory, ((base->quant + 1) + storage_size) * data_size);
        #ifdef DEBUG_AE
            printf("Allocated %x\n", p);
        #endif
        if (p == NULL)
            return 3;

        base->memory = p;
    }

    memmove(base->memory + (i + 1) * data_size, base->memory + i * data_size, (base->quant - i) * data_size);
    memmove(base->memory + i * data_size, par, data_size);

    base->quant++;

    return 0;
}

uint8_t f_delete_ae_base(ae_base *base, size_t data_size, size_t storage_size, size_t i, void *par)
{
    if (base->memory == NULL)
        return 1;

    if ((base->quant + 1) % storage_size == 0)
    {
        void *p = realloc(base->memory, ((base->quant + 1) + storage_size) * data_size);
        #ifdef DEBUG_AE
            printf("Allocated %x\n", p);
        #endif
        if (p == NULL)
            return 2;

        base->memory = p;
    }

    if (par != NULL)
        memmove(par, base->memory + i * data_size, data_size);

    memmove(base->memory + i * data_size, base->memory + (i + 1) * data_size, (base->quant - i) * data_size);

    base->quant--;

    return 0;
}

uint8_t f_duplicate_ae_base(ae_base *base_out, ae_base *base_in, size_t data_size, size_t storage_size)
{
    if (base_out->memory != NULL)
        f_free_ae_base(base_out);

    if (f_create_ae_base(base_out, data_size, storage_size) != 0)
        return 1;
    if (f_resize_ae_base(base_out, data_size, storage_size, base_in->quant) != 0)
        return 2;

    if (base_in->memory != NULL)
        memmove(base_out->memory, base_in->memory, base_in->quant * data_size);

    return 0;
}

uint8_t f_invert_ae_base(ae_base *base, size_t data_size)
{
    if (base->memory == NULL)
        return 1;

    if (base->quant > 1)
    {
        size_t iterations = base->quant / 2;
        size_t i = 0;
        void *par = malloc(data_size);
        if (par == NULL)
            return 2;

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

uint8_t f_find_ae_base(ae_base *base, size_t data_size, void *par, size_t *ret)
{
    if (base->memory == NULL)
        return 1;
    if (par == NULL)
        return 2;

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